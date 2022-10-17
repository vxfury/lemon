#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <fstream>

#if 0
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define TOSTRING(line) #line
#define LOCATION(file, line) \
    &file ":" TOSTRING(line)[(__builtin_strrchr(file, '/') ? (__builtin_strrchr(file, '/') - file + 1) : 0)]

#define PARSERS_TRACE(fmt, ...)                                                                         \
    do {                                                                                                \
        char buff[32];                                                                                  \
        struct tm tm;                                                                                   \
        struct timeval tv;                                                                              \
        gettimeofday(&tv, NULL);                                                                        \
        localtime_r(&tv.tv_sec, &tm);                                                                   \
        size_t len = strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm);                            \
        snprintf(&buff[len], sizeof(buff) - len, ".%03d", ((tv.tv_usec + 500) / 1000) % 1000);          \
        printf("\033[2;3m%s\033[0m <%s> " fmt "\n", buff, LOCATION(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while (0);
#else
#define PARSERS_TRACE(fmt, ...)
#endif

namespace parsers
{
enum {
    INI_OK = 0,       //!< No error
    INI_UPDATED = 1,  //!< An existing value was updated
    INI_INSERTED = 2, //!< A new value was inserted
    INI_FAIL = -1,    //!< Generic failure
    INI_NOMEM = -2,   //!< Out of memory error
    INI_FILE = -3     //!< File error (see errno for detail error)
};

#define INI_NEWLINE        "\n"
#define INI_UTF8_SIGNATURE "\xEF\xBB\xBF"

template <typename Compare>
class INIParserPattern {
  public:
    struct Entry {
        const char *item;
        const char *comment;
        int order;

        Entry(const char *item = NULL, int order = 0) : item(item), comment(NULL), order(order) {}
        Entry(const char *item, const char *comment, int order) : item(item), comment(comment), order(order) {}
        Entry(const Entry &rhs)
        {
            operator=(rhs);
        }
        Entry &operator=(const Entry &rhs)
        {
            item = rhs.item;
            comment = rhs.comment;
            order = rhs.order;
            return *this;
        }

        /** Strict less ordering by name of key only */
        struct KeyOrder {
            bool operator()(const Entry &lhs, const Entry &rhs) const
            {
                const static Compare isLess = Compare();
                return isLess(lhs.item, rhs.item);
            }
        };

        /** Strict less ordering by order, and then name of key */
        struct LoadOrder {
            bool operator()(const Entry &lhs, const Entry &rhs) const
            {
                if (lhs.order != rhs.order) {
                    return lhs.order < rhs.order;
                } else {
                    return KeyOrder()(lhs.item, rhs.item);
                }
            }
        };
    };
    // set of dependent string pointers.
    // Note that these pointers are dependent on memory owned by INIParserPattern
    using Entries = typename std::list<Entry>;

    // map keys to values
    using Pairs = std::multimap<Entry, const char *, typename Entry::KeyOrder>;

    // map sections to key/value map
    using Sections = std::map<Entry, Pairs, typename Entry::KeyOrder>;

  public:
    INIParserPattern(bool use_utf8 = false, bool allow_multikey = false, bool allow_multiline = false)
        : m_text(0),
          m_textlen(0),
          m_filecomment(NULL),
          m_is_utf8(use_utf8),
          m_allow_multikey(allow_multikey),
          m_allow_multiline(allow_multiline),
          m_add_spaces(true),
          m_order(0)
    {
    }
    ~INIParserPattern()
    {
        Reset();
    }

    /** Deallocate all memory stored by this object */
    void Reset()
    {
        // remove all data
        delete[] m_text;
        m_text = NULL;
        m_textlen = 0;
        m_filecomment = NULL;
        if (!m_data.empty()) {
            m_data.erase(m_data.begin(), m_data.end());
        }

        // remove all strings
        if (!m_strings.empty()) {
            typename Entries::iterator i = m_strings.begin();
            for (; i != m_strings.end(); ++i) {
                delete[] const_cast<char *>(i->item);
            }
            m_strings.erase(m_strings.begin(), m_strings.end());
        }
    }

    /** Has any data been loaded */
    bool IsEmpty() const
    {
        return m_data.empty();
    }

    bool IsUnicode() const
    {
        return m_is_utf8;
    }
    void SetUnicode(bool use_utf8 = true)
    {
        if (!m_text) m_is_utf8 = use_utf8;
    }

    bool IsMultiKey() const
    {
        return m_allow_multikey;
    }
    void SetMultiKey(bool allow_multikey = true)
    {
        m_allow_multikey = allow_multikey;
    }

    bool IsMultiLine() const
    {
        return m_allow_multiline;
    }
    void SetMultiLine(bool allow_multiline = true)
    {
        m_allow_multiline = allow_multiline;
    }

    void SetSpaces(bool add_spaces = true)
    {
        m_add_spaces = add_spaces;
    }

    /** Query the status of spaces output */
    bool UsingSpaces() const
    {
        return m_add_spaces;
    }

    int LoadFile(const char *file)
    {
        std::ifstream rf(file, std::ios::in | std::ios::binary);
        if (!rf.good()) {
            return INI_FILE;
        }

        std::string loaded((std::istreambuf_iterator<char>(rf)), std::istreambuf_iterator<char>());
        if (loaded.empty() || !rf.good()) {
            return -errno;
        }

        return LoadText(loaded);
    }

    int LoadText(const std::string &text)
    {
        return LoadText(text.c_str(), text.size());
    }

    int LoadText(const char *text, size_t textlen)
    {
        if (!text) {
            return INI_OK;
        }

        // if the UTF-8 BOM exists, consume it and set mode to unicode, if we have
        // already loaded data and try to change mode half-way through then this will
        // be ignored and we will assert in debug versions
        if (textlen >= 3 && memcmp(text, INI_UTF8_SIGNATURE, 3) == 0) {
            text += 3;
            textlen -= 3;
            assert(m_is_utf8 || !m_text); // we don't expect mixed mode data
            SetUnicode();
        }

        if (textlen == 0) {
            return INI_OK;
        }
        if (textlen == (size_t)(-1)) {
            return INI_FAIL;
        }

        // allocate memory for the data, ensure that there is a NULL
        // terminator wherever the converted data ends
        char *pData = new (std::nothrow) char[textlen + 1];
        if (!pData) {
            return INI_NOMEM;
        }
        memset(pData, 0, sizeof(char) * (textlen + 1));
        memcpy(pData, text, textlen);

        // parse it
        const static char empty = 0;
        char *pWork = pData;
        const char *pSection = &empty;
        const char *item = NULL;
        const char *pVal = NULL;
        const char *comment = NULL;

        // We copy the strings if we are loading data into this class when we
        // already have stored some.
        bool bCopyStrings = (m_text != NULL);

        // find a file comment if it exists, this is a comment that starts at the
        // beginning of the file and continues until the first blank line.
        int rc = FindFileComment(pWork, bCopyStrings);
        if (rc < 0) return rc;

        // add every entry in the file to the data table
        while (FindEntry(pWork, pSection, item, pVal, comment)) {
            rc = AddEntry(pSection, item, pVal, comment, false, bCopyStrings);
            if (rc < 0) return rc;
        }

        // store these strings if we didn't copy them
        if (bCopyStrings) {
            delete[] pData;
        } else {
            m_text = pData;
            m_textlen = textlen + 1;
        }

        return INI_OK;
    }

    int DumpFile(const char *file, bool add_signature = true) const
    {
        int err;
        std::string dumped;
        if ((err = Dump(dumped, add_signature)) == 0) {
            std::string tmpfile = std::string(file) + ".saving."
                                  + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                                       std::chrono::system_clock::now().time_since_epoch())
                                                       .count());
            std::ofstream wf(tmpfile, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!wf.good()) {
                return -errno;
            }
            wf.write(dumped.data(), dumped.size());
            wf.close();
            if (rename(tmpfile.c_str(), file) != 0) {
                return -errno;
            }
        }

        return err;
    }

    int Dump(std::string &dumpped, bool add_signature = false) const
    {
        // add the UTF-8 signature if it is desired
        if (m_is_utf8 && add_signature) {
            dumpped += INI_UTF8_SIGNATURE;
        }

        // get all of the sections sorted in load order
        Entries oSections;
        GetAllSections(oSections);
        oSections.sort(typename Entry::LoadOrder());

        // if there is an empty section name, then it must be written out first
        // regardless of the load order
        typename Entries::iterator is = oSections.begin();
        for (; is != oSections.end(); ++is) {
            if (!*is->item) {
                // move the empty section name to the front of the section list
                if (is != oSections.begin()) {
                    oSections.splice(oSections.begin(), oSections, is, std::next(is));
                }
                break;
            }
        }

        // write the file comment if we have one
        bool bNeedNewLine = false;
        if (m_filecomment) {
            if (!OutputMultiLineText(dumpped, m_filecomment)) {
                return INI_FAIL;
            }
            bNeedNewLine = true;
        }

        // iterate through our sections and output the data
        typename Entries::const_iterator iSection = oSections.begin();
        for (; iSection != oSections.end(); ++iSection) {
            // write out the comment if there is one
            if (iSection->comment) {
                if (bNeedNewLine) {
                    dumpped += INI_NEWLINE INI_NEWLINE;
                }
                if (!OutputMultiLineText(dumpped, iSection->comment)) {
                    return INI_FAIL;
                }
                bNeedNewLine = false;
            }

            if (bNeedNewLine) {
                dumpped += INI_NEWLINE INI_NEWLINE;
                bNeedNewLine = false;
            }

            // write the section (unless there is no section name)
            if (*iSection->item) {
                dumpped += "[";
                dumpped += iSection->item;
                dumpped += "]";
                dumpped += INI_NEWLINE;
            }

            // get all of the keys sorted in load order
            Entries oKeys;
            GetAllKeys(iSection->item, oKeys);
            oKeys.sort(typename Entry::LoadOrder());

            // write all keys and values
            typename Entries::const_iterator iKey = oKeys.begin();
            for (; iKey != oKeys.end(); ++iKey) {
                // get all values for this key
                Entries oValues;
                GetAllValues(iSection->item, iKey->item, oValues);

                typename Entries::const_iterator iValue = oValues.begin();
                for (; iValue != oValues.end(); ++iValue) {
                    // write out the comment if there is one
                    if (iValue->comment) {
                        dumpped += INI_NEWLINE;
                        if (!OutputMultiLineText(dumpped, iValue->comment)) {
                            return INI_FAIL;
                        }
                    }

                    // write the key
                    dumpped += iKey->item;
                    PARSERS_TRACE("Dump Key = %s", iKey->item);

                    // write the value
                    dumpped += m_add_spaces ? " = " : "=";
                    if (m_allow_multiline && IsMultiLineData(iValue->item)) {
                        // multi-line data needs to be processed specially to ensure
                        // that we use the correct newline format for the current system
                        dumpped += "<<<END_OF_TEXT" INI_NEWLINE;
                        if (!OutputMultiLineText(dumpped, iValue->item)) {
                            return INI_FAIL;
                        }
                        PARSERS_TRACE("Dump Value = %s", iValue->item);
                        dumpped += "END_OF_TEXT";
                    } else {
                        dumpped += iValue->item;
                    }
                    dumpped += INI_NEWLINE;
                }
            }

            bNeedNewLine = true;
        }

        return INI_OK;
    }

    void GetAllSections(Entries &entries) const
    {
        entries.clear();
        for (auto it = m_data.cbegin(); it != m_data.cend(); ++it) {
            entries.push_back(it->first);
        }
    }

    bool GetAllKeys(const char *section, Entries &entries) const
    {
        entries.clear();
        if (!section) {
            return false;
        }

        auto it = m_data.find(section);
        if (it == m_data.end()) {
            return false;
        }

        const Pairs &keys = it->second;
        const char *lastkey = NULL;
        typename Pairs::const_iterator iKeyVal = keys.begin();
        for (int n = 0; iKeyVal != keys.end(); ++iKeyVal, ++n) {
            if (!lastkey || IsLess(lastkey, iKeyVal->first.item)) {
                entries.push_back(iKeyVal->first);
                lastkey = iKeyVal->first.item;
            }
        }

        return true;
    }

    bool GetAllValues(const char *section, const char *key, Entries &entries) const
    {
        entries.clear();
        if (!section || !key) {
            return false;
        }
        auto it = m_data.find(section);
        if (it == m_data.end()) {
            return false;
        }
        auto keyit = it->second.find(key);
        if (keyit == it->second.end()) {
            return false;
        }

        // insert all values for this key
        entries.emplace_back(keyit->second, keyit->first.comment, keyit->first.order);
        if (m_allow_multikey) {
            ++keyit;
            while (keyit != it->second.end() && !IsLess(key, keyit->first.item)) {
                entries.emplace_back(keyit->second, keyit->first.comment, keyit->first.order);
                ++keyit;
            }
        }

        return true;
    }

    int GetSectionSize(const char *section) const
    {
        if (!section) {
            return -1;
        }

        typename Sections::const_iterator iSection = m_data.find(section);
        if (iSection == m_data.end()) {
            return -1;
        }
        const Pairs &keys = iSection->second;

        // if multi-key isn't permitted then the keys size is
        // the number of keys that we have.
        if (!m_allow_multikey || keys.empty()) {
            return (int)keys.size();
        }

        // otherwise we need to count them
        int nCount = 0;
        const char *pLastKey = NULL;
        typename Pairs::const_iterator iKeyVal = keys.begin();
        for (int n = 0; iKeyVal != keys.end(); ++iKeyVal, ++n) {
            if (!pLastKey || IsLess(pLastKey, iKeyVal->first.item)) {
                ++nCount;
                pLastKey = iKeyVal->first.item;
            }
        }
        return nCount;
    }

    const Pairs *GetSection(const char *section) const
    {
        if (section) {
            typename Sections::const_iterator i = m_data.find(section);
            if (i != m_data.end()) {
                return &(i->second);
            }
        }
        return 0;
    }

    const char *GetValue(const char *section, const char *key, const char *default_value = NULL,
                         bool *has_multiple = NULL) const
    {
        if (has_multiple) {
            *has_multiple = false;
        }
        if (!section || !key) {
            return default_value;
        }
        typename Sections::const_iterator iSection = m_data.find(section);
        if (iSection == m_data.end()) {
            return default_value;
        }
        typename Pairs::const_iterator iKeyVal = iSection->second.find(key);
        if (iKeyVal == iSection->second.end()) {
            return default_value;
        }

        // check for multiple entries with the same key
        if (m_allow_multikey && has_multiple) {
            typename Pairs::const_iterator iTemp = iKeyVal;
            if (++iTemp != iSection->second.end()) {
                if (!IsLess(key, iTemp->first.item)) {
                    *has_multiple = true;
                }
            }
        }

        return iKeyVal->second;
    }

    long GetLongValue(const char *section, const char *key, long default_value = 0, bool *has_multiple = NULL) const
    {
        // return the default if we don't have a value
        const char *pszValue = GetValue(section, key, NULL, has_multiple);
        if (!pszValue || !*pszValue) return default_value;

        // convert to UTF-8/MBCS which for a numeric value will be the same as ASCII
        char szValue[64] = {0};
        memcpy(szValue, pszValue, strlen(pszValue));

        // handle the value as hex if prefaced with "0x"
        long nValue = default_value;
        char *pszSuffix = szValue;
        if (szValue[0] == '0' && (szValue[1] == 'x' || szValue[1] == 'X')) {
            if (!szValue[2]) return default_value;
            nValue = strtol(&szValue[2], &pszSuffix, 16);
        } else {
            nValue = strtol(szValue, &pszSuffix, 10);
        }

        // any invalid strings will return the default value
        if (*pszSuffix) {
            return default_value;
        }

        return nValue;
    }

    double GetDoubleValue(const char *section, const char *key, double default_value = 0,
                          bool *has_multiple = NULL) const
    {
        // return the default if we don't have a value
        const char *pszValue = GetValue(section, key, NULL, has_multiple);
        if (!pszValue || !*pszValue) return default_value;

        char *pszSuffix = NULL;
        double nValue = strtod(pszValue, &pszSuffix);

        // any invalid strings will return the default value
        if (!pszSuffix || *pszSuffix) {
            return default_value;
        }

        return nValue;
    }

    bool GetBoolValue(const char *section, const char *key, bool a_bDefault = false, bool *has_multiple = NULL) const
    {
        // return the default if we don't have a value
        const char *pszValue = GetValue(section, key, NULL, has_multiple);
        if (!pszValue || !*pszValue) return a_bDefault;

        // we only look at the minimum number of characters
        switch (pszValue[0]) {
            case 't':
            case 'T': // true
            case 'y':
            case 'Y': // yes
            case '1': // 1 (one)
                return true;

            case 'f':
            case 'F': // false
            case 'n':
            case 'N': // no
            case '0': // 0 (zero)
                return false;

            case 'o':
            case 'O':
                if (pszValue[1] == 'n' || pszValue[1] == 'N') return true;  // on
                if (pszValue[1] == 'f' || pszValue[1] == 'F') return false; // off
                break;
        }

        // no recognized value, return the default
        return a_bDefault;
    }

    int SetValue(const char *section, const char *key, const char *value, const char *comment = NULL,
                 bool replace = false)
    {
        return AddEntry(section, key, value, comment, replace, true);
    }

    int SetLongValue(const char *section, const char *key, long value, const char *comment = NULL, bool usehex = false,
                     bool replace = false)
    {
        // use SetValue to create sections
        if (!section || !key) return INI_FAIL;

        // convert to an ASCII string
        char szInput[64];
        snprintf(szInput, sizeof(szInput), usehex ? "0x%lx" : "%ld", value);

        // actually add it
        return AddEntry(section, key, szInput, comment, replace, true);
    }

    int SetDoubleValue(const char *section, const char *key, double value, const char *comment = NULL,
                       bool replace = false)
    {
        // use SetValue to create sections
        if (!section || !key) return INI_FAIL;

        // convert to an ASCII string
        char szInput[64];
        snprintf(szInput, sizeof(szInput), "%f", value);

        // actually add it
        return AddEntry(section, key, szInput, comment, replace, true);
    }

    int SetBoolValue(const char *section, const char *key, bool a_bValue, const char *comment = NULL,
                     bool replace = false)
    {
        // use SetValue to create sections
        if (!section || !key) return INI_FAIL;

        // convert to an ASCII string
        const char *pszInput = a_bValue ? "true" : "false";

        // actually add it
        return AddEntry(section, key, pszInput, comment, replace, true);
    }

    bool Delete(const char *section, const char *key, bool remove_empty = false)
    {
        return DeleteValue(section, key, NULL, remove_empty);
    }

    bool DeleteValue(const char *section, const char *key, const char *value, bool remove_empty = false)
    {
        if (!section) {
            return false;
        }

        auto sectionit = m_data.find(section);
        if (sectionit == m_data.end()) {
            return false;
        }

        // remove a single key if we have a keyname
        if (key) {
            typename Pairs::iterator pairit = sectionit->second.find(key);
            if (pairit == sectionit->second.end()) {
                return false;
            }

            const static Compare isLess = Compare();

            // remove any copied strings and then the key
            bool deleted = false;
            typename Pairs::iterator delit;
            do {
                delit = pairit++;
                if (value == NULL || (isLess(value, delit->second) == false && isLess(delit->second, value) == false)) {
                    DeleteString(delit->first.item);
                    DeleteString(delit->second);
                    sectionit->second.erase(delit);
                    deleted = true;
                }
            } while (pairit != sectionit->second.end() && !IsLess(key, pairit->first.item));

            if (!deleted) {
                return false;
            }

            // done now if the section is not empty or we are not pruning away
            // the empty sections. Otherwise let it fall through into the section
            // deletion code
            if (!remove_empty || !sectionit->second.empty()) {
                return true;
            }
        } else {
            // delete all copied strings from this section. The actual
            // entries will be removed when the section is removed.
            typename Pairs::iterator pairit = sectionit->second.begin();
            for (; pairit != sectionit->second.end(); ++pairit) {
                DeleteString(pairit->first.item);
                DeleteString(pairit->second);
            }
        }

        // delete the section itself
        DeleteString(sectionit->first.item);
        m_data.erase(sectionit);

        return true;
    }

  private:
    // copying is not permitted
    INIParserPattern(const INIParserPattern &);            // disabled
    INIParserPattern &operator=(const INIParserPattern &); // disabled

    int FindFileComment(char *&text, bool clone)
    {
        // there can only be a single file comment
        if (m_filecomment) {
            return INI_OK;
        }

        // Load the file comment as multi-line text, this will modify all of
        // the newline characters to be single \n chars
        if (!LoadMultiLineText(text, m_filecomment, NULL, false)) {
            return INI_OK;
        }

        // copy the string if necessary
        if (clone) {
            int rc = CopyString(m_filecomment);
            if (rc < 0) return rc;
        }

        return INI_OK;
    }

    bool FindEntry(char *&text, const char *&section, const char *&key, const char *&value, const char *&comment) const
    {
        comment = NULL;

        char *pTrail = NULL;
        while (*text) {
            // skip spaces and empty lines
            while (*text && IsSpace(*text)) {
                ++text;
            }
            if (!*text) {
                break;
            }

            // skip processing of comment lines but keep a pointer to
            // the start of the comment.
            if (IsComment(*text)) {
                LoadMultiLineText(text, comment, NULL, true);
                continue;
            }

            // process section names
            if (*text == '[') {
                // skip leading spaces
                ++text;
                while (*text && IsSpace(*text)) {
                    ++text;
                }

                // find the end of the section name (it may contain spaces)
                // and convert it to lowercase as necessary
                section = text;
                while (*text && *text != ']' && !IsNewLineChar(*text)) {
                    ++text;
                }

                // if it's an invalid line, just skip it
                if (*text != ']') {
                    continue;
                }

                // remove trailing spaces from the section
                pTrail = text - 1;
                while (pTrail >= section && IsSpace(*pTrail)) {
                    --pTrail;
                }
                ++pTrail;
                *pTrail = 0;

                // skip to the end of the line
                ++text; // safe as checked that it == ']' above
                while (*text && !IsNewLineChar(*text)) {
                    ++text;
                }

                key = NULL;
                value = NULL;
                return true;
            }

            // find the end of the key name (it may contain spaces)
            // and convert it to lowercase as necessary
            key = text;
            while (*text && *text != '=' && !IsNewLineChar(*text)) {
                ++text;
            }

            // if it's an invalid line, just skip it
            if (*text != '=') {
                continue;
            }

            // empty keys are invalid
            if (key == text) {
                while (*text && !IsNewLineChar(*text)) {
                    ++text;
                }
                continue;
            }

            // remove trailing spaces from the key
            pTrail = text - 1;
            while (pTrail >= key && IsSpace(*pTrail)) {
                --pTrail;
            }
            ++pTrail;
            *pTrail = 0;

            // skip leading whitespace on the value
            ++text; // safe as checked that it == '=' above
            while (*text && !IsNewLineChar(*text) && IsSpace(*text)) {
                ++text;
            }

            // find the end of the value which is the end of this line
            value = text;
            while (*text && !IsNewLineChar(*text)) {
                ++text;
            }

            // remove trailing spaces from the value
            pTrail = text - 1;
            if (*text) { // prepare for the next round
                SkipNewLine(text);
            }
            while (pTrail >= value && IsSpace(*pTrail)) {
                --pTrail;
            }
            ++pTrail;
            *pTrail = 0;

            // check for multi-line entries
            if (m_allow_multiline && IsMultiLineTag(value)) {
                // skip the "<<<" to get the tag that will end the multiline
                const char *pTagName = value + 3;
                return LoadMultiLineText(text, value, pTagName);
            }

            // return the standard entry
            return true;
        }

        return false;
    }

    int AddEntry(const char *section, const char *key, const char *value, const char *comment, bool replace, bool clone)
    {
        int rc;
        bool inserted = false;

        assert(!comment || IsComment(*comment));

        // if we are copying strings then make a copy of the comment now
        // because we will need it when we add the entry.
        if (clone && comment) {
            rc = CopyString(comment);
            if (rc < 0) return rc;
        }

        // create the section entry if necessary
        typename Sections::iterator iSection = m_data.find(section);
        if (iSection == m_data.end()) {
            // if the section doesn't exist then we need a copy as the
            // string needs to last beyond the end of this function
            if (clone) {
                rc = CopyString(section);
                if (rc < 0) return rc;
            }

            // only set the comment if this is a section only entry
            Entry oSection(section, ++m_order);
            if (comment && (!key || !value)) {
                oSection.comment = comment;
            }

            typename Sections::value_type oEntry(oSection, Pairs());
            typedef typename Sections::iterator SectionIterator;
            std::pair<SectionIterator, bool> i = m_data.insert(oEntry);
            iSection = i.first;
            inserted = true;
        }
        if (!key || !value) {
            // section only entries are specified with item and pVal as NULL
            return inserted ? INI_INSERTED : INI_UPDATED;
        }

        // check for existence of the key
        Pairs &keyval = iSection->second;
        typename Pairs::iterator iKey = keyval.find(key);
        inserted = iKey == keyval.end();

        // remove all existing entries but save the load order and
        // comment of the first entry
        int nLoadOrder = ++m_order;
        if (iKey != keyval.end() && m_allow_multikey && replace) {
            const char *comment_ = NULL;
            while (iKey != keyval.end() && !IsLess(key, iKey->first.item)) {
                if (iKey->first.order < nLoadOrder) {
                    nLoadOrder = iKey->first.order;
                    comment_ = iKey->first.comment;
                }
                ++iKey;
            }
            if (comment_) {
                DeleteString(comment);
                comment = comment_;
                CopyString(comment);
            }
            Delete(section, key);
            iKey = keyval.end();
        }

        // make string copies if necessary
        bool force_create_key = m_allow_multikey && !replace;
        if (clone) {
            if (force_create_key || iKey == keyval.end()) {
                // if the key doesn't exist then we need a copy as the
                // string needs to last beyond the end of this function
                // because we will be inserting the key next
                rc = CopyString(key);
                if (rc < 0) return rc;
            }

            // we always need a copy of the value
            rc = CopyString(value);
            if (rc < 0) return rc;
        }

        // create the key entry
        if (iKey == keyval.end() || force_create_key) {
            Entry oKey(key, nLoadOrder);
            if (comment) {
                oKey.comment = comment;
            }
            typename Pairs::value_type oEntry(oKey, static_cast<const char *>(NULL));
            iKey = keyval.insert(oEntry);
        }

        iKey->second = value;
        return inserted ? INI_INSERTED : INI_UPDATED;
    }

    /** Is the supplied character a whitespace character? */
    inline bool IsSpace(char ch) const
    {
        return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
    }

    /** Does the supplied character start a comment line? */
    inline bool IsComment(char ch) const
    {
        return (ch == ';' || ch == '#');
    }

    /** Skip over a newline character (or characters) for either DOS or UNIX */
    inline void SkipNewLine(char *&text) const
    {
        text += (*text == '\r' && *(text + 1) == '\n') ? 2 : 1;
    }

    /** Make a copy of the supplied string, replacing the original pointer */
    int CopyString(const char *&str)
    {
        char *cloned = strdup(str);
        if (cloned == nullptr) {
            return INI_NOMEM;
        }
        str = cloned;
        m_strings.push_back(cloned);

        return INI_OK;
    }

    /** Delete a string from the copied strings buffer if necessary */
    void DeleteString(const char *str)
    {
        // strings may exist either inside the data block, or they will be
        // individually allocated and stored in m_strings. We only physically
        // delete those stored in m_strings.
        if (str < m_text || str >= m_text + m_textlen) {
            typename Entries::iterator i = m_strings.begin();
            for (; i != m_strings.end(); ++i) {
                if (str == i->item) {
                    delete[] const_cast<char *>(i->item);
                    m_strings.erase(i);
                    break;
                }
            }
        }
    }

    /** Internal use of our string comparison function */
    bool IsLess(const char *left, const char *right) const
    {
        const static Compare isLess = Compare();
        return isLess(left, right);
    }

    bool IsMultiLineTag(const char *text) const
    {
        // check for the "<<<" prefix for a multi-line entry
        if (*text++ != '<') return false;
        if (*text++ != '<') return false;
        if (*text++ != '<') return false;
        return true;
    }

    bool IsMultiLineData(const char *text) const
    {
        // data is multi-line if it has any of the following features:
        //  * whitespace prefix
        //  * embedded newlines
        //  * whitespace suffix

        // empty string
        if (!*text) {
            return false;
        }

        // check for prefix
        if (IsSpace(*text)) {
            return true;
        }

        // embedded newlines
        while (*text) {
            if (IsNewLineChar(*text)) {
                return true;
            }
            ++text;
        }

        // check for suffix
        if (IsSpace(*--text)) {
            return true;
        }

        return false;
    }

    bool LoadMultiLineText(char *&text, const char *&value, const char *tagname,
                           bool allow_blank_lines_in_comment = false) const
    {
        // we modify this data to strip all newlines down to a single '\n'
        // character. This means that on Windows we need to strip out some
        // characters which will make the data shorter.
        // i.e.  LINE1-LINE1\r\nLINE2-LINE2\0 will become
        //       LINE1-LINE1\nLINE2-LINE2\0
        // The pDataLine entry is the pointer to the location in memory that
        // the current line needs to start to run following the existing one.
        // This may be the same as pCurrLine in which case no move is needed.
        char *pDataLine = text;
        char *pCurrLine;

        // value starts at the current line
        value = text;

        // find the end tag. This tag must start in column 1 and be
        // followed by a newline. We ignore any whitespace after the end
        // tag but not whitespace before it.
        char cEndOfLineChar = *text;
        for (;;) {
            // if we are loading comments then we need a comment character as
            // the first character on every line
            if (!tagname && !IsComment(*text)) {
                // if we aren't allowing blank lines then we're done
                if (!allow_blank_lines_in_comment) {
                    break;
                }

                // if we are allowing blank lines then we only include them
                // in this comment if another comment follows, so read ahead
                // to find out.
                char *pCurr = text;
                int nNewLines = 0;
                while (IsSpace(*pCurr)) {
                    if (IsNewLineChar(*pCurr)) {
                        ++nNewLines;
                        SkipNewLine(pCurr);
                    } else {
                        ++pCurr;
                    }
                }

                // we have a comment, add the blank lines to the output
                // and continue processing from here
                if (IsComment(*pCurr)) {
                    for (; nNewLines > 0; --nNewLines) *pDataLine++ = '\n';
                    text = pCurr;
                    continue;
                }

                // the comment ends here
                break;
            }

            // find the end of this line
            pCurrLine = text;
            while (*text && !IsNewLineChar(*text)) ++text;

            // move this line down to the location that it should be if necessary
            if (pDataLine < pCurrLine) {
                size_t nLen = (size_t)(text - pCurrLine);
                memmove(pDataLine, pCurrLine, nLen * sizeof(char));
                pDataLine[nLen] = '\0';
            }

            // end the line with a NULL
            cEndOfLineChar = *text;
            *text = 0;

            // if are looking for a tag then do the check now. This is done before
            // checking for end of the data, so that if we have the tag at the end
            // of the data then the tag is removed correctly.
            if (tagname) {
                // strip whitespace from the end of this tag
                char *pc = text - 1;
                while (pc > pDataLine && IsSpace(*pc)) --pc;
                char ch = *++pc;
                *pc = 0;

                if (!IsLess(pDataLine, tagname) && !IsLess(tagname, pDataLine)) {
                    break;
                }

                *pc = ch;
            }

            // if we are at the end of the data then we just automatically end
            // this entry and return the current data.
            if (!cEndOfLineChar) {
                return true;
            }

            // otherwise we need to process this newline to ensure that it consists
            // of just a single \n character.
            pDataLine += (text - pCurrLine);
            *text = cEndOfLineChar;
            SkipNewLine(text);
            *pDataLine++ = '\n';
        }

        // if we didn't find a comment at all then return false
        if (value == text) {
            value = NULL;
            return false;
        }

        // the data (which ends at the end of the last line) needs to be
        // null-terminated BEFORE before the newline character(s). If the
        // user wants a new line in the multi-line data then they need to
        // add an empty line before the tag.
        *--pDataLine = '\0';

        // if looking for a tag and if we aren't at the end of the data,
        // then move text to the start of the next line.
        if (tagname && cEndOfLineChar) {
            assert(IsNewLineChar(cEndOfLineChar));
            *text = cEndOfLineChar;
            SkipNewLine(text);
        }

        return true;
    }
    bool IsNewLineChar(char a_c) const
    {
        return (a_c == '\n' || a_c == '\r');
    }

    bool OutputMultiLineText(std::string &dumpped, const char *text) const
    {
        const char *endofline;
        char endoflinechar = *text;
        while (endoflinechar) {
            // find the end of this line
            endofline = text;
            while (*endofline && *endofline != '\n') {
                ++endofline;
            }
            endoflinechar = *endofline;
            *const_cast<char *>(endofline) = 0;
            dumpped += text;
            dumpped += INI_NEWLINE;
            *const_cast<char *>(endofline) = endoflinechar;
            text += (endofline - text) + 1;
        }
        return true;
    }

  private:
    /** Copy of the INI file data in our character format. This will be
        modified when parsed to have NULL characters added after all
        interesting string entries. All of the string pointers to sections,
        keys and values point into this block of memory.
     */
    char *m_text;

    /** Length of the data that we have stored. Used when deleting strings
        to determine if the string is stored here or in the allocated string
        buffer.
     */
    size_t m_textlen;

    /** File comment for this data, if one exists. */
    const char *m_filecomment;

    /** Parsed INI data. Section -> (Key -> Value). */
    Sections m_data;

    /** This vector stores allocated memory for copies of strings that have
        been supplied after the file load. It will be empty unless SetValue()
        has been called.
     */
    Entries m_strings;

    /** Is the format of our datafile UTF-8 or MBCS? */
    bool m_is_utf8;

    /** Are multiple values permitted for the same key? */
    bool m_allow_multikey;

    /** Are data values permitted to span multiple lines? */
    bool m_allow_multiline;

    /** Should spaces be written out surrounding the equals sign? */
    bool m_add_spaces;

    /** Next order value, used to ensure sections and keys are output in the
        same order that they are loaded/added.
     */
    int m_order;
};

/**
 * Generic case-sensitive less than comparison. This class returns numerically
 * ordered ASCII case-sensitive text for all possible sizes and types of
 * char.
 */
struct INI_GenericCase {
    bool operator()(const char *pLeft, const char *pRight) const
    {
        long cmp;
        for (; *pLeft && *pRight; ++pLeft, ++pRight) {
            cmp = (long)*pLeft - (long)*pRight;
            if (cmp != 0) {
                return cmp < 0;
            }
        }
        return *pRight != 0;
    }
};

/**
 * Generic ASCII case-insensitive less than comparison. This class returns
 * numerically ordered ASCII case-insensitive text for all possible sizes
 * and types of char. It is not safe for MBCS text comparison where
 * ASCII A-Z characters are used in the encoding of multi-byte characters.
 */
struct INI_GenericNoCase {
    bool operator()(const char *pLeft, const char *pRight) const
    {
        long cmp;
        for (; *pLeft && *pRight; ++pLeft, ++pRight) {
            cmp = (long)std::tolower(*pLeft) - (long)std::tolower(*pRight);
            if (cmp != 0) {
                return cmp < 0;
            }
        }
        return *pRight != 0;
    }
};

using INIParser = typename parsers::INIParserPattern<INI_GenericNoCase>;
using INICaseParser = typename parsers::INIParserPattern<INI_GenericCase>;
} // namespace parsers

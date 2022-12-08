#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace edit_distance
{
// Returns the optimal edits to go from 'left' to 'right'.
// All edits cost the same, with replace having lower priority than
// add/remove.
// Simple implementation of the Wagner-Fischer algorithm.
// See http://en.wikipedia.org/wiki/Wagner-Fischer_algorithm
enum EditType { kMatch, kAdd, kRemove, kReplace };
std::vector<EditType> CalculateOptimalEdits(const std::vector<size_t> &left, const std::vector<size_t> &right)
{
    std::vector<std::vector<double> > costs(left.size() + 1, std::vector<double>(right.size() + 1));
    std::vector<std::vector<EditType> > best_move(left.size() + 1, std::vector<EditType>(right.size() + 1));

    // Populate for empty right.
    for (size_t l_i = 0; l_i < costs.size(); ++l_i) {
        costs[l_i][0] = static_cast<double>(l_i);
        best_move[l_i][0] = kRemove;
    }
    // Populate for empty left.
    for (size_t r_i = 1; r_i < costs[0].size(); ++r_i) {
        costs[0][r_i] = static_cast<double>(r_i);
        best_move[0][r_i] = kAdd;
    }

    for (size_t l_i = 0; l_i < left.size(); ++l_i) {
        for (size_t r_i = 0; r_i < right.size(); ++r_i) {
            if (left[l_i] == right[r_i]) {
                // Found a match. Consume it.
                costs[l_i + 1][r_i + 1] = costs[l_i][r_i];
                best_move[l_i + 1][r_i + 1] = kMatch;
                continue;
            }

            const double add = costs[l_i + 1][r_i];
            const double remove = costs[l_i][r_i + 1];
            const double replace = costs[l_i][r_i];
            if (add < remove && add < replace) {
                costs[l_i + 1][r_i + 1] = add + 1;
                best_move[l_i + 1][r_i + 1] = kAdd;
            } else if (remove < add && remove < replace) {
                costs[l_i + 1][r_i + 1] = remove + 1;
                best_move[l_i + 1][r_i + 1] = kRemove;
            } else {
                // We make replace a little more expensive than add/remove to lower
                // their priority.
                costs[l_i + 1][r_i + 1] = replace + 1.00001;
                best_move[l_i + 1][r_i + 1] = kReplace;
            }
        }
    }

    // Reconstruct the best path. We do it in reverse order.
    std::vector<EditType> best_path;
    for (size_t l_i = left.size(), r_i = right.size(); l_i > 0 || r_i > 0;) {
        EditType move = best_move[l_i][r_i];
        best_path.push_back(move);
        l_i -= move != kAdd;
        r_i -= move != kRemove;
    }
    std::reverse(best_path.begin(), best_path.end());
    return best_path;
}

namespace
{
// Helper class to convert string into ids with deduplication.
class StringNumbering {
  public:
    size_t GetId(const std::string &str)
    {
        auto it = ids_.find(str);
        if (it != ids_.end()) return it->second;
        return ids_[str] = ids_.size();
    }

  private:
    std::map<std::string, size_t> ids_;
};
} // namespace

std::vector<EditType> CalculateOptimalEdits(const std::vector<std::string> &left, const std::vector<std::string> &right)
{
    std::vector<size_t> left_ids, right_ids;
    {
        StringNumbering intern_table;
        for (auto const &str : left) {
            left_ids.push_back(intern_table.GetId(str));
        }
        for (auto const &str : right) {
            right_ids.push_back(intern_table.GetId(str));
        }
    }
    return CalculateOptimalEdits(left_ids, right_ids);
}

namespace
{
// Helper class that holds the state for one hunk and prints it out to the
// stream.
// It reorders adds/removes when possible to group all removes before all
// adds. It also adds the hunk header before printint into the stream.
class Hunk {
  public:
    Hunk(size_t left_start, size_t right_start)
        : left_start_(left_start), right_start_(right_start), adds_(), removes_(), common_()
    {
    }

    void PushLine(char edit, const char *line)
    {
        switch (edit) {
            case ' ':
                ++common_;
                FlushEdits();
                hunk_.push_back(std::make_pair(' ', line));
                break;
            case '-':
                ++removes_;
                hunk_removes_.push_back(std::make_pair('-', line));
                break;
            case '+':
                ++adds_;
                hunk_adds_.push_back(std::make_pair('+', line));
                break;
        }
    }

    void PrintTo(std::ostream *os)
    {
        PrintHeader(os);
        FlushEdits();
        for (std::list<std::pair<char, const char *> >::const_iterator it = hunk_.begin(); it != hunk_.end(); ++it) {
            *os << it->first << it->second << "\n";
        }
    }

    bool has_edits() const
    {
        return adds_ || removes_;
    }

  private:
    void FlushEdits()
    {
        hunk_.splice(hunk_.end(), hunk_removes_);
        hunk_.splice(hunk_.end(), hunk_adds_);
    }

    // Print a unified diff header for one hunk.
    // The format is
    //   "@@ -<left_start>,<left_length> +<right_start>,<right_length> @@"
    // where the left/right parts are omitted if unnecessary.
    void PrintHeader(std::ostream *ss) const
    {
        *ss << "@@ ";
        if (removes_) {
            *ss << "-" << left_start_ << "," << (removes_ + common_);
        }
        if (removes_ && adds_) {
            *ss << " ";
        }
        if (adds_) {
            *ss << "+" << right_start_ << "," << (adds_ + common_);
        }
        *ss << " @@\n";
    }

    size_t left_start_, right_start_;
    size_t adds_, removes_, common_;
    std::list<std::pair<char, const char *> > hunk_, hunk_adds_, hunk_removes_;
};
} // namespace

// Create a list of diff hunks in Unified diff format.
// Each hunk has a header generated by PrintHeader above plus a body with
// lines prefixed with ' ' for no change, '-' for deletion and '+' for addition.
// 'context' represents the desired unchanged prefix/suffix around the diff.
// If two hunks are close enough that their contexts overlap, then they are
// joined into one hunk.
std::string CreateUnifiedDiff(const std::vector<std::string> &left, const std::vector<std::string> &right,
                              size_t context)
{
    const std::vector<EditType> edits = CalculateOptimalEdits(left, right);

    size_t l_i = 0, r_i = 0, edit_i = 0;
    std::stringstream ss;
    while (edit_i < edits.size()) {
        // Find first edit.
        while (edit_i < edits.size() && edits[edit_i] == kMatch) {
            ++l_i;
            ++r_i;
            ++edit_i;
        }

        // Find the first line to include in the hunk.
        const size_t prefix_context = std::min(l_i, context);
        Hunk hunk(l_i - prefix_context + 1, r_i - prefix_context + 1);
        for (size_t i = prefix_context; i > 0; --i) {
            hunk.PushLine(' ', left[l_i - i].c_str());
        }

        // Iterate the edits until we found enough suffix for the hunk or the input
        // is over.
        size_t n_suffix = 0;
        for (; edit_i < edits.size(); ++edit_i) {
            if (n_suffix >= context) {
                // Continue only if the next hunk is very close.
                auto it = edits.begin() + static_cast<int>(edit_i);
                while (it != edits.end() && *it == kMatch) ++it;
                if (it == edits.end() || static_cast<size_t>(it - edits.begin()) - edit_i >= context) {
                    // There is no next edit or it is too far away.
                    break;
                }
            }

            EditType edit = edits[edit_i];
            // Reset count when a non match is found.
            n_suffix = edit == kMatch ? n_suffix + 1 : 0;

            if (edit == kMatch || edit == kRemove || edit == kReplace) {
                hunk.PushLine(edit == kMatch ? ' ' : '-', left[l_i].c_str());
            }
            if (edit == kAdd || edit == kReplace) {
                hunk.PushLine('+', right[r_i].c_str());
            }

            // Advance indices, depending on edit type.
            l_i += edit != kAdd;
            r_i += edit != kRemove;
        }

        if (!hunk.has_edits()) {
            // We are done. We don't want this hunk.
            break;
        }

        hunk.PrintTo(&ss);
    }
    return ss.str();
}
} // namespace edit_distance

int main()
{
    std::vector<std::string> str1 = {"line1", "hello world", "line3"};
    std::vector<std::string> str2 = {"line1", "hello world2", "line3", "line4"};
    std::cout << edit_distance::CreateUnifiedDiff(str1, str2, 2) << std::endl;

    return 0;
}

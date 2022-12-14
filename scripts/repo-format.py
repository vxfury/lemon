#!/usr/local/bin/python3
# -*- coding: UTF-8 -*-

import os
import re
import sys
import time
import datetime
import subprocess
import multiprocessing
import git
import typing
import itertools
import argparse
import string
import tty
import ctypes
import termios
import xml.etree.ElementTree as ET

GITCONF = git.GitConfigParser(
    [os.path.normpath(os.path.expanduser("~/.gitconfig"))],
    read_only = True
)

COPYRIGHT = """/**
 * Copyright {0} {1}({2})
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */\n\n""".format(
    datetime.datetime.now().year,
    GITCONF.get_value('user', 'name'), GITCONF.get_value('user', 'email')
 ).encode()


# tqdm has bugs.
# CSI sequence: https://www.liquisearch.com/ansi_escape_code/csi_codes
class ProgressBar:
    """
    Progress Bar

    ...

    Attributes
    ----------
    disable: bool
        True to disable progress bar (default False)
    position: tuple(int, int)
        the number of legs the animal has (default 4)
    progress: float[0.0, 100.0]
        percentage of progress
    max_width: int
        max width of entire progress bar (default terminal.columns)
    bar_width: int
        bar width of progress bar (default max_width // 2)

    Methods
    -------
    advance(progress)
        Inc progress and refresh display
    set_progress(progress)
        Set progress and refresh display
    set_args(**kwargs)
        Set arguments and refresh display
    add_args(**kwargs)
        Add arguments and refresh display
    clear()
        Clear display once time
    """

    def __init__(
        self,
        format: str = "{progress} {elapsed} | {bar} | {remaining}",
        position: tuple[int, int] = None,
        disable: bool = False,
        leave: bool = True,
        total: float = 100.0,
        max_width: int = 0,
        bar_width: int = 0,
        file=sys.stdout,
        keep_cursor_hidden: bool = False,
        manager: multiprocessing.Manager = None,
        **kwargs,
    ):
        """
        format: progress bar format, internal keys: progress, elapsed, bar, remaining
        position: position of bar
        disable: True to hide the progress bar
        leave: True to leave the progress bar after exit
        total: max size to advance
        max_width: max width of entire progress bar
        bar_width: bar width
        file: destination of progress bar (default sys.stdout)
        keep_cursor_hidden: True to keep cursor hidden after exit
        manager: multiprocessing support
        kwargs: custom key-value pairs
        """
        self.__file = file
        self.__leave = leave
        self.__total = total
        self.__format = format
        self.__kwargs = kwargs
        self.__starttime = time.time()
        self.__keep_cursor_hidden = keep_cursor_hidden
        self.__lock = None if manager is None else manager.Lock()
        self.__progress = 0.0 if manager is None else manager.Value(ctypes.c_float, 0.0)

        self.disable = disable
        self.position = position
        self.max_width = max_width if max_width else os.get_terminal_size().columns - 8
        self.bar_width = bar_width if bar_width != 0 and bar_width < self.max_width else self.max_width // 2

        self.__display()  # CSI?25l: Hides the cursor

    def __del__(self):
        if not self.disable:
            outputs = ""
            if self.position:
                # `CSI n ; m H`: Moves the cursor to row n, column m
                outputs += f"\x1B[{self.position[0]};{self.position[1]}H"
            if not self.__leave:
                # `CSI n k`: Erases part of the line
                outputs += "\x1B[2K"  # n=2, clear entire line
            else:
                outputs += "\x1B[E"
            if not self.__keep_cursor_hidden:
                # `CSI ? 25 h``: Shows the cursor
                outputs += "\x1B[?25h"
            self.__write(outputs + "\r")

    def clear(self):
        """
        clear display once time
        """
        if not self.disable:
            outputs = ""
            if self.position:
                # `CSI n ; m H`: Moves the cursor to row n, column m
                outputs += f"\x1B[{self.position[0]};{self.position[1]}H"
            outputs += "\x1B[2K\r"  # n=2, clear entire line
            self.__write(outputs)

    def advance(self, step):
        """
        advance progress by <100.0 * step / total>
        """
        if self.__lock:
            self.__lock.acquire()
        last_progress = self.progress
        if last_progress != self.__inc_progress(100.0 * step / self.__total):
            self.__display()
        if self.__lock:
            self.__lock.release()

    @property
    def progress(self):
        """
        get progress
        """
        if isinstance(self.__progress, float):
            return self.__progress
        else:
            return self.__progress.value

    @progress.setter
    def progress(self, val):
        self.set_progress(val)

    def __set_progress(self, progress):
        if isinstance(self.__progress, float):
            self.__progress = min(progress, 100.0)
            return self.__progress
        else:
            self.__progress.value = min(progress, 100.0)
            return self.__progress.value

    def __inc_progress(self, progress):
        if isinstance(self.__progress, float):
            self.__progress = min(self.__progress + progress, 100.0)
            return self.__progress
        else:
            self.__progress.value = min(self.__progress.value + progress, 100.0)
            return self.__progress.value

    def set_progress(self, progress):
        """
        set progress to <progress>
        """
        if self.__lock:
            self.__lock.acquire()
        if self.progress != self.__set_progress(progress):
            self.__display()
        if self.__lock:
            self.__lock.release()

    def set_args(self, **kwargs):
        if kwargs != self.__kwargs:
            self.__kwargs = kwargs
            self.__display()

    def add_args(self, **kwargs):
        self.__kwargs.update(kwargs)
        self.__display()

    def __display(self):
        def format_time(t):
            """
            Formats a number of seconds as a clock time, [H:]MM:SS
            """
            mins, s = divmod(int(t), 60)
            h, m = divmod(mins, 60)
            if mins == 0 and t > 0.1 and s == 0:
                s = 1
            if h:
                return "{0:d}:{1:02d}:{2:02d}".format(h, m, s)
            else:
                return "{0:02d}:{1:02d}".format(m, s)

        def format_bar(p, w):
            l = int(w * p / 100.0) if p < 99.9 else w
            return ">" * l + " " * (w - l)

        def format_all(format, **kwargs):
            class BlankFormatter(string.Formatter):
                def __init__(self, default=""):
                    self.default = default

                def get_value(self, key, args, kwds):
                    if isinstance(key, str):
                        return kwds.get(key, self.default)
                    else:
                        return string.Formatter.get_value(key, args, kwds)

            text = BlankFormatter().format(format, **kwargs).rstrip()
            while text.endswith(":"):
                text = text[: len(text) - 1].rstrip()
            return text

        if not self.disable:
            now = time.time()
            self.__kwargs["progress"] = "{0:5.1f}%".format(self.progress)
            self.__kwargs["bar"] = format_bar(self.progress, self.bar_width)

            self.__kwargs["elapsed"] = format_time(now - self.__starttime)
            self.__kwargs["remaining"] = (
                format_time((now - self.__starttime) * (100 - self.progress) / self.progress)
                if self.progress
                else "--:--"
            )

            outputs = "\x1B[?25l"
            if self.position:
                # `CSIn;mH`: Moves the cursor to row n, column m
                outputs += f"\x1B[s\x1B[{self.position[0]};{self.position[1]}H"
            else:
                outputs += "\r"

            line = format_all(self.__format, **self.__kwargs)
            if self.max_width != 0:
                if len(line) > self.max_width:
                    truncated = "..."
                    if self.max_width >= len(truncated):
                        line = line[: (self.max_width - len(truncated))] + truncated + "\x1b[0m"
                    else:
                        line = line[: self.max_width] + "\x1b[0m"
            outputs += line + "\x1B[0K"
            if self.position:
                outputs += "\x1B[u"
            self.__write(outputs)

    def __write(self, str):
        file = self.__file if self.__file else sys.stdout
        file.write(str)
        file.flush()

    @staticmethod
    def getpos():
        buf = ""
        stdin = sys.stdin.fileno()
        tattr = termios.tcgetattr(stdin)

        try:
            tty.setcbreak(stdin, termios.TCSANOW)
            sys.stdout.write("\x1b[6n")
            sys.stdout.flush()

            while True:
                buf += sys.stdin.read(1)
                if buf[-1] == "R":
                    break

        finally:
            termios.tcsetattr(stdin, termios.TCSANOW, tattr)

        # reading the actual values, but what if a keystroke appears while reading
        # from stdin? As dirty work around, getpos() returns if this fails: None
        try:
            matches = re.match(r"^\x1b\[(\d*);(\d*)R", buf)
            groups = matches.groups()
        except AttributeError:
            return None

        return (int(groups[0]), int(groups[1]))


class ProgressBars:
    def __init__(
        self,
        format,
        size,
        max_width=os.get_terminal_size().columns,
        bar_width=0,
        disable=False,
        leave=False,
        initial_pos=None,
        overall_total=0,
        manager: multiprocessing.Manager = None,
    ):
        offset = 0
        if overall_total != 0:
            self.overall = ProgressBar(
                leave=True,
                disable=disable,
                total=overall_total,
                file=None,
                max_width=max_width,
                bar_width=bar_width,
                position=initial_pos,
                keep_cursor_hidden=True,
                manager=manager,
            )
            offset = 1
        else:
            self.overall = None

        self.bars = [
            ProgressBar(
                format,
                disable=disable,
                file=None,
                max_width=max_width,
                bar_width=bar_width,
                leave=True,  # keep bar if not all bars end
                position=((initial_pos[0] + i), initial_pos[1]) if initial_pos else None,
                keep_cursor_hidden=True,
                manager=manager,
            )
            for i in range(offset, size + offset)
        ]
        self.__leave = leave
        self.disable = disable
        self.initial_pos = initial_pos

    def __del__(self):
        off = 0 if self.overall is None else 1
        if self.__leave:
            off += len(self.bars)
            self.bars = []
        else:
            [bar.clear() for bar in self.bars]

        if not self.disable:
            outputs = ""
            if self.initial_pos:
                # `CSI n ; m H`: Moves the cursor to row n, column m
                outputs += f"\x1b[s\x1B[2K\x1B[{self.initial_pos[0] + off};{self.initial_pos[1]}H"

            outputs += "\x1B[?25h"
            sys.stdout.write(outputs)
            sys.stdout.flush()

    def __getitem__(self, index):
        return self.bars[index % len(self.bars)]


"""a replacement instance within a single file"""


class Replacement:
    def __init__(self, offset, length, content, path=None):
        self.offset = offset
        self.length = length
        self.content = content
        self.path = path

    def __hash__(self):
        return hash((self.offset, self.length))

    def __eq__(self, other):
        return (self.offset, self.length) == (other.offset, other.length)

    def __repr__(self):
        return f"offset={self.offset} " f"length={self.length} content={self.content}"

    def get_affected_commit(self, file, repo: git.Repo, blame_info, bar: ProgressBar):
        def offset_to_line(file, offset):
            file.seek(0)
            content = file.read(offset)
            return content.decode("utf-8", "ignore").count("\n") + 1

        def map_of_line_commit(blame_info):
            result = {}
            current_line = 0
            for one_blame in blame_info:
                for one_line in one_blame[1]:
                    current_line += 1
                    result[current_line] = one_blame
            return result

        first_line_offset = offset_to_line(file, self.offset)
        last_line_offset = offset_to_line(file, max(self.offset, self.offset + self.length - 1))

        # fetch blame information for this replacement.
        original_blames = []
        line_commit_map = map_of_line_commit(blame_info)

        for i in range(first_line_offset, last_line_offset + 1):
            if i >= len(line_commit_map):
                # apply blame of first line if out of range, the last line mostly
                original_blames.append(line_commit_map[1][0])
                continue

            if line_commit_map[i][0] not in original_blames:
                original_blames.append(line_commit_map[i][0])

        if len(original_blames) > 1 and original_blames[0].author != original_blames[1].author:
            bar.add_args(
                desc=f"\x1B[33m{first_line_offset}-{last_line_offset} modified by {list(map(lambda b: str(b.author), original_blames))}. regard as modified by {original_blames[-1].author}\x1B[0m"
            )

        return original_blames[-1]

    """apply replacement onto file."""

    def apply(self, file):
        # seek to offset + length where content is untouched by self
        file.seek(self.offset + self.length)

        # read till the EOF
        b = file.read()

        # then insert 'content' at the start and write back
        file.seek(self.offset)
        file.write(self.content + b)

        # truncate unnecessary content in case content is shorter than origin
        file.truncate()

    def _filter_trivial_blame(self, blame):
        for line in blame[1]:
            if not line.isspace() and len(line) and not self.filter_trivial_line(line):
                return True
        return False

    """tells base class if given source line is considered trivial"""

    def filter_trivial_line(self, line):
        return False


class ReplacementGenerator:
    """generates replacements for a file."""

    def generate(self, file, *args, **kwargs) -> typing.List[Replacement]:
        pass


class ClangFormatReplacement(Replacement):
    trivial_line_pattern = re.compile(r"^[\{\}\[\]\(\);]+$")

    def filter_trivial_line(self, line):
        # in C-like languages, braces ({}), brackets ([]), parentheses (())
        # and semicolon (;) are often arranged into a single line in favor of
        # some codestyles, on which blame infomation is usually useless.
        l = line.translate(dict.fromkeys(map(ord, string.whitespace)))
        return self.__class__.trivial_line_pattern.match(l)


class ClangFormatXMLReplacementGenerator(ReplacementGenerator):
    def generate(self, file, bar: ProgressBar) -> typing.List[Replacement]:
        file.seek(0, os.SEEK_END)
        size = file.tell()
        bar.add_args(desc="generate clang-format XML replacements ...")
        output = subprocess.check_output(
            ("clang-format", "--output-replacements-xml", "--style=file", file.name)
        ).decode("utf-8")
        bar.set_progress(100 * (1024 + size * 2 / 4) / (1024 + size + 4096))

        def add_copyrigth_auto(file, result):
            file.seek(0)
            if len(COPYRIGHT) > 0 and b"Copyright" not in file.read(300):
                if len(result) > 0 and result[0].offset == 0:
                    result[0].content = COPYRIGHT + result[0].content
                else:
                    r = ClangFormatReplacement(0, 0, COPYRIGHT)
                    result.insert(0, r)

        result = []
        xml_root = ET.fromstring(output)
        for r in xml_root.iterfind("replacement"):
            offset = int(r.get("offset"))
            length = int(r.get("length"))
            byte_content = bytes(r.text or "", encoding="utf-8")
            bar.add_args(desc=f"replacement of lines {offset}-{offset + length}")
            bar.set_progress(100 * (1024 + (offset + length) * 3 / 5) / (1024 + size + 4096))

            # clang-format produces replacements overlapped with original text, which results in
            # multi-commit blame results. remove overlapped areas for better result.
            i = len(byte_content)

            if i > 0:
                i = i - 1
                file.seek(offset + length - 1)

                while i >= 0 and length > 0 and file.read(1)[0] == byte_content[i]:
                    i = i - 1
                    length = length - 1
                    file.seek(offset + length - 1)

                byte_content = byte_content[: i + 1]

                i = 0
                file.seek(offset)
                while i < len(byte_content) and length > 0 and file.read(1)[0] == byte_content[i]:
                    i = i + 1
                    length = length - 1
                    offset = offset + 1
                    file.seek(offset)

                byte_content = byte_content[i:]

            result.append(ClangFormatReplacement(offset, length, byte_content))

        add_copyrigth_auto(file, result)
        bar.set_progress(100 * (1024 + size * 3 / 5) / (1024 + size + 4096))

        return result


# binary search
def binary_search(arr, comparator):
    return _binary_search(arr, 0, len(arr) - 1, comparator)


def _binary_search(arr, l, r, comparator):
    if r < l:
        return -1

    mid = int(l + (r - l) / 2)
    compare = comparator(arr[mid])

    if compare == 0:
        return mid
    elif compare < 0:
        return _binary_search(arr, l, mid - 1, comparator)
    else:
        return _binary_search(arr, mid + 1, r, comparator)


def generate_replacements(repo: git.Repo, path, limit, bar: ProgressBar, overall_bar: ProgressBar):
    replacements_by_paths = {}
    replacements_by_affected_commits = {}
    bar.set_args(
        file=f"{os.path.relpath(path, os.path.dirname(repo.common_dir) if repo else os.getcwd())}",
        desc="...",
    )
    try:
        size = os.path.getsize(path)

        if size > limit * 1024:
            bar.add_args(desc=f"\x1B[33m{size // 1024}KB, more than {limit} KB, skipped\x1B[0m")
            return None, None
        if size <= 1:
            bar.add_args(desc=f"\x1B[33mempty file, skipped\x1B[0m")
            return None, None

        bar.set_progress(100 * 1024 / (1024 + size / 4096))
        with open(path, mode="rb") as file:
            bar.add_args(desc=f"generate replacements")
            replacements_for_cur_file = ClangFormatXMLReplacementGenerator().generate(file, bar=bar)

            if not replacements_for_cur_file:
                bar.add_args(desc=f"no replacements found")
                return None, None

            replacements_for_cur_file.sort(key=lambda r: r.offset)
            replacements_by_paths[path] = replacements_for_cur_file

            if repo:
                try:
                    blame_info = repo.blame(repo.head.commit, file.name)
                except git.exc.GitCommandError as e:
                    if "is outside repository" in e.stderr:
                        # file not in repository, ignore
                        bar.add_args(desc="outside repository")
                        return replacements_by_paths, None
                    else:
                        return replacements_by_paths, None

                # fetch blame information for each replacement,
                # then make a <affected_commits> -> <replacements> map.
                for r in replacements_for_cur_file:
                    r.path = path
                    affected_commit = r.get_affected_commit(file, repo, blame_info, bar)

                    # affected_commits = blame_result[1] if len(blame_result[1]) else blame_result[0]
                    # affected_commits = list(map(lambda b: b[0].binsha.hex(), affected_commits))
                    affected_commit = affected_commit.binsha.hex()

                    replacements = replacements_by_affected_commits.get(affected_commit, [])
                    replacements.append(r)
                    replacements_by_affected_commits[affected_commit] = replacements
    except PermissionError as e:
        bar.add_args(desc=f"\x1B[33mpermission denied, skipped\x1B[0m")
    finally:
        overall_bar.advance(1)
        bar.set_progress(100.0)

    return (
        replacements_by_paths,
        replacements_by_affected_commits if len(replacements_by_affected_commits) != 0 else None,
    )


def repo_format_main(initial_pos):
    starttime = time.time()

    # arguments parser
    parser = argparse.ArgumentParser(
        description="Generate and apply replacements for existing Git "
        "repository, without altering future blame results"
    )

    parser.add_argument("--repo", required=False, help="path to git repository, sub-path is acceptable")
    parser.add_argument("--path", required=False, help="path to format, default is path to repo")
    parser.add_argument("--exclude", required=False, help="directory don't format")

    parser.add_argument("--no-commit", "-n", action="store_true", help="no commit, just format file")

    parser.add_argument("--copyright", required=False, help="copyright")
    parser.add_argument(
        "--dry-run",
        required=False,
        default=False,
        action="store_true",
        help="calculate files those will be formated but no formating actually",
    )
    parser.add_argument(
        "--jobs", "-j", required=False, default=multiprocessing.cpu_count(), help="jobs to do formatting"
    )
    parser.add_argument("--limit", required=False, default=256, help="max KB of file to format")
    parser.add_argument(
        "--quiet",
        required=False,
        default=False,
        action="store_true",
        help="quiet mode",
    )
    parser.add_argument("--author", required=False, default=os.getlogin(), help="author for unkown commits")
    parser.add_argument(
        "--filter-by-author", required=False, default=None, help="only format commits by specified author"
    )

    # BEGIN OF MAIN: validate given arguments.
    args = parser.parse_args()
    if args.copyright is not None:
        COPYRIGHT = args.copyright

    repo = git.Repo(args.repo, search_parent_directories=True) if args.repo else None
    format_path = os.path.expanduser(args.repo if args.path is None else args.path)
    if not os.path.isabs(format_path):
        format_path = os.path.join(os.getcwd(), format_path)

    files = []
    if os.path.isdir(format_path):
        for root, dirs, file_list in os.walk(format_path):

            def need_exclude(root, excludes):
                for dir in excludes:
                    if os.path.abspath(os.path.join(format_path, dir)).startswith(
                        os.path.abspath(os.path.expanduser(root))
                    ):
                        return True

                return False

            if args.exclude and need_exclude(root, args.exclude.split(",")):
                # print(f"Exclude {root}, {args.exclude}")
                continue

            for filename in file_list:
                if filename.endswith((".cpp", ".cc", ".h", ".hpp", ".c")):
                    files.append(os.path.join(root, filename))

    elif os.path.isfile(format_path):
        files.append(format_path)

    # process files specified in arguments. new commits will be created during processing,
    # so we cache current head for accurate blaming results.
    with multiprocessing.Pool(processes=int(args.jobs)) as pool:
        manager = multiprocessing.Manager()
        size = min(int(args.jobs), multiprocessing.cpu_count() // 2, len(files))
        bars = ProgressBars(
            "{progress} {elapsed} | {file}: {desc}",
            size,
            leave=False,
            disable=bool(args.quiet),
            initial_pos=(initial_pos[0] + 1, initial_pos[1]),
            overall_total=len(files),
            manager=manager,
        )

        results = pool.starmap_async(
            generate_replacements,
            [(repo, path, int(args.limit), bars[i], bars.overall) for i, path in enumerate(files)],
        ).get()
        pool.close()
        pool.join()
        del bars  # delete bars for elegant print

    replacements_by_paths = {}
    replacements_by_paths_only = {}
    replacements_by_affected_commits = {}
    for replacements_by_path, replacements_by_affected_commit in results:
        if replacements_by_path and replacements_by_affected_commit is None:
            replacements_by_paths_only.update(replacements_by_path)
        else:
            if replacements_by_path:
                replacements_by_paths.update(replacements_by_path)
            if replacements_by_affected_commit:
                for affected_commit in replacements_by_affected_commit.keys():
                    if not affected_commit in replacements_by_affected_commits.keys():
                        replacements_by_affected_commits[affected_commit] = []
                    replacements_by_affected_commits[affected_commit].extend(
                        replacements_by_affected_commit[affected_commit]
                    )

    if repo and not args.dry_run:
        # combine affected commits with same author.
        replacements_by_authors = {}
        affected_commits_by_authors = {}

        for commit_hex, replacements in replacements_by_affected_commits.items():
            first_commit = repo.commit(commit_hex)
            author = first_commit.author
            affected_commits = affected_commits_by_authors.get(author, set())
            affected_commits.add(commit_hex)
            affected_commits_by_authors[author] = affected_commits

        for author, commits_by_author in affected_commits_by_authors.items():
            affected_commits = set()
            replacements = []
            if args.filter_by_author and author != author:
                continue
            for one_commit in commits_by_author:
                affected_commits.add(one_commit)
                replacements.extend(replacements_by_affected_commits[one_commit])
            replacements_by_authors[tuple(affected_commits)] = replacements

        # apply replacements, one affected commit(s) by one.
        for commits, replacements in replacements_by_authors.items():
            file_paths = set()
            for r in replacements:
                with open(r.path, mode="rb+") as file:
                    r.apply(file)
                    file_paths.add(file.name)

                length_delta = len(r.content) - r.length
                replacements_in_same_file = replacements_by_paths[r.path]
                if length_delta != 0 and len(replacements_in_same_file):
                    # apply replacement each time, and adjuest offset of replacements after it
                    index = binary_search(replacements_in_same_file, lambda re: r.offset - re.offset)
                    for re in itertools.islice(replacements_in_same_file, index, None):
                        re.offset = re.offset + length_delta

                    # remove current replacement for future better search performance.
                    del replacements_in_same_file[index]
                    replacements_by_paths[r.path] = replacements_in_same_file

            if not args.no_commit and len(file_paths) != 0:
                repo.index.add(file_paths, False)

                # done with current commit, make new one
                first_commit = repo.commit(commits[0])

                # multiple commits are affected:
                # lossless replacement is impossible (automatically).
                # explicitly create a new commit and try to retain infomation
                # from original commits as much as possible.
                commit_msg = f"repo-format: reformat and keep blame info"
                repo.index.commit(commit_msg, author=first_commit.author)

    if not args.dry_run and replacements_by_paths_only:
        file_paths = set(replacements_by_paths_only.keys())
        for path, replacements in replacements_by_paths_only.items():
            for r in replacements:
                with open(path, mode="rb+") as file:
                    r.apply(file)
                    file_paths.add(file.name)

                length_delta = len(r.content) - r.length
                replacements_in_same_file = replacements_by_paths_only[path]
                if length_delta != 0 and len(replacements_in_same_file):
                    # apply replacement each time, and adjuest offset of replacements after it
                    index = binary_search(replacements_in_same_file, lambda re: r.offset - re.offset)
                    for re in itertools.islice(replacements_in_same_file, index, None):
                        re.offset = re.offset + length_delta

                    # remove current replacement for future better search performance.
                    del replacements_in_same_file[index]
                    replacements_by_paths[path] = replacements_in_same_file

        if repo and not args.no_commit and len(file_paths) != 0:
            repo.index.add(file_paths, False)
            commit_msg = f"repo-format: format uncommit codes"
            repo.git.commit("-m", commit_msg, author=args.author)

    print(
        f"Time elapsed: {round(time.time() - starttime, 2)} s."
        + f" {len(list(filter(lambda r: r[0] is not None, results)))} of {len(files)} file(s) "
        + ("can be formatted." if args.dry_run else "formatted.")
    )


import random


def progress_bar_main():
    bar = ProgressBar(leave=True, widht=90, total=500, bar_width=60)
    for i in range(0, 500):
        bar.advance(1)
        time.sleep(0.005)


def progress_bar_test(length, bar, overall_bar):
    remain = length
    while remain > 0:
        len = random.randint(0, remain)
        remain -= len
        time.sleep(0.001)
        bar.set_progress(100 * (length - remain) / length)
    overall_bar.advance(1)


def progress_bars_main(initial_pos):
    manager = multiprocessing.Manager()
    with multiprocessing.Pool(processes=6) as pool:
        size = 3
        total = 10000
        initial_pos = initial_pos
        bars = ProgressBars(
            "{progress} {elapsed} | {bar} | {remaining}",
            size,
            leave=True,
            initial_pos=(initial_pos[0] + 1, initial_pos[1]),
            overall_total=total,
            manager=manager,
        )

        pool.starmap_async(
            progress_bar_test,
            [(random.randint(1000, 10000), bars[i], bars.overall) for i in range(total)],
        ).get()
        pool.close()
        pool.join()


if __name__ == "__main__":
    initial_pos = ProgressBar.getpos()
    # initial_pos = (initial_pos[0] - 1, initial_pos[1])
    # repo_format_main(initial_pos)
    progress_bar_main()
    progress_bars_main(initial_pos)

""" .clang-format
---
Language: Cpp
AccessModifierOffset: '-2'
AlignAfterOpenBracket: Align
AlignConsecutiveMacros: 'true'
AlignConsecutiveAssignments: 'false'
AlignConsecutiveDeclarations: 'false'
AlignEscapedNewlines: Left
AlignOperands: 'true'
AlignTrailingComments: 'true'
AllowAllArgumentsOnNextLine: 'true'
AllowAllConstructorInitializersOnNextLine: 'true'
AllowAllParametersOfDeclarationOnNextLine: 'true'
AllowShortBlocksOnASingleLine: 'false'
AllowShortCaseLabelsOnASingleLine: 'false'
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: WithoutElse
AllowShortLambdasOnASingleLine: None
AllowShortLoopsOnASingleLine: 'true'
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
AlwaysBreakBeforeMultilineStrings: 'false'
AlwaysBreakTemplateDeclarations: 'Yes'
BinPackArguments: 'true'
BinPackParameters: 'true'
BraceWrapping:
  AfterCaseLabel: 'false'
  AfterClass: 'false'
  AfterControlStatement: 'false'
  AfterEnum: 'false'
  AfterFunction: 'true'
  AfterNamespace: 'true'
  AfterObjCDeclaration: 'false'
  AfterStruct: 'false'
  AfterUnion: 'false'
  AfterExternBlock: 'false'
  BeforeCatch: 'false'
  BeforeElse: 'false'
  IndentBraces: 'false'
  SplitEmptyFunction: 'true'
  SplitEmptyRecord: 'true'
  SplitEmptyNamespace: 'true'
BreakBeforeBinaryOperators: NonAssignment
BreakBeforeBraces: Custom
BreakBeforeTernaryOperators: 'true'
BreakConstructorInitializers: BeforeColon
BreakInheritanceList: BeforeColon
BreakStringLiterals: 'true'
ColumnLimit: '120'
CompactNamespaces: 'false'
ConstructorInitializerAllOnOneLineOrOnePerLine: 'true'
ConstructorInitializerIndentWidth: '4'
ContinuationIndentWidth: '4'
Cpp11BracedListStyle: 'true'
DerivePointerAlignment: 'false'
DisableFormat: 'false'
ExperimentalAutoDetectBinPacking: 'false'
FixNamespaceComments: 'true'
ForEachMacros: ['foreach', 'FOREACH', 'RANGES_FOR', 'hlist_for_each_entry_continue', 'hlist_for_each_entry', 'hlist_for_each_entry_from', 'hlist_for_each_entry_safe', 'hlist_for_each_safe', 'list_for_each_entry', 'list_for_each_entry_continue', 'list_for_each_entry_continue_reverse', 'list_for_each_entry_from', 'list_for_each_entry_reverse', 'list_for_each_entry_safe', 'list_for_each_entry_safe_continue', 'list_for_each_entry_safe_from', 'list_for_each_entry_safe_reverse', 'list_for_each_from', 'list_for_each_prev', 'list_for_each_prev_safe', 'list_for_each_safe']
TypenameMacros: ['STACK_OF', 'LIST']
IncludeBlocks: Regroup
IncludeIsMainRegex: '([-_](test|unittest))?$'
IndentCaseLabels: 'true'
IndentPPDirectives: None
IndentWidth: '4'
IndentWrappedFunctionNames: 'false'
KeepEmptyLinesAtTheStartOfBlocks: 'false'
MaxEmptyLinesToKeep: '3'
NamespaceIndentation: None
PenaltyBreakAssignment: '2'
PenaltyBreakBeforeFirstCallParameter: '1'
PenaltyBreakComment: '300'
PenaltyBreakFirstLessLess: '120'
PenaltyBreakString: '1000'
PenaltyBreakTemplateDeclaration: '10'
PenaltyExcessCharacter: '1000000'
PenaltyReturnTypeOnItsOwnLine: '500'
PointerAlignment: Right
RawStringFormats:
  - Language: Cpp
    Delimiters:
      - 'cc'
      - 'CC'
      - 'cpp'
      - 'Cpp'
      - 'CPP'
      - 'c++'
      - 'C++'
    CanonicalDelimiter: ''
    BasedOnStyle: google
  - Language: TextProto
    Delimiters:
      - 'pb'
      - 'PB'
      - 'proto'
      - 'PROTO'
    EnclosingFunctions:
      - EqualsProto
      - EquivToProto
      - PARSE_PARTIAL_TEXT_PROTO
      - PARSE_TEST_PROTO
      - PARSE_TEXT_PROTO
      - ParseTextOrDie
      - ParseTextProtoOrDie
    CanonicalDelimiter: ''
    BasedOnStyle: google
ReflowComments: 'true'
SortIncludes: 'false'
SortUsingDeclarations: 'false'
SpaceAfterCStyleCast: 'false'
SpaceAfterLogicalNot: 'false'
SpaceAfterTemplateKeyword: 'true'
SpaceBeforeAssignmentOperators: 'true'
SpaceBeforeCpp11BracedList: 'false'
SpaceBeforeCtorInitializerColon: 'true'
SpaceBeforeInheritanceColon: 'true'
SpaceBeforeParens: ControlStatements
SpaceBeforeRangeBasedForLoopColon: 'true'
SpaceInEmptyParentheses: 'false'
SpacesBeforeTrailingComments: '1'
SpacesInAngles: 'false'
SpacesInCStyleCastParentheses: 'false'
SpacesInContainerLiterals: 'false'
SpacesInParentheses: 'false'
SpacesInSquareBrackets: 'false'
Standard: Auto
StatementMacros: ['__maybe_unused']
TabWidth: '4'
UseTab: Never
...
"""

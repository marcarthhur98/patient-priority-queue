"""Compile fresh C binaries and exercise public commands and memory ownership."""
import os
from pathlib import Path
import subprocess
import unittest

ROOT = Path(__file__).resolve().parent
BUILD = ROOT / 'build'
SUFFIX = '.exe' if os.name == 'nt' else ''
APP = BUILD / ('patient-queue' + SUFFIX)
MEMORY = BUILD / ('test-memory' + SUFFIX)


class QueueTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        BUILD.mkdir(exist_ok=True)
        for source, target in [('lab9.c', APP), ('test_memory.c', MEMORY)]:
            subprocess.run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            str(ROOT / source), '-o', str(target)], check=True)

    def run_commands(self, commands):
        process = subprocess.run([str(APP)], input=commands, text=True,
                                 capture_output=True, timeout=3)
        self.assertEqual(process.returncode, 0, process.stderr)
        return process.stdout

    def ids(self, output):
        return [int(line.split()[1]) for line in output.splitlines() if line.startswith('ID:')]

    def test_priority_fifo_and_treat(self):
        result = self.run_commands('A 1 Alex 2\nA 2 Blair 5\nA 3 Casey 5\nD\nT\nD\nQ\n')
        self.assertEqual(self.ids(result), [2, 3, 1, 3, 1])
        self.assertIn('Patient 2 Treated.', result)

    def test_duplicates_and_remove_head_middle_tail(self):
        result = self.run_commands('A 1 Alex 5\nA 2 Blair 3\nA 3 Casey 1\n'
                                   'A 1 Duplicate 4\nR 2\nD\nR 3\nD\nR 1\nD\nQ\n')
        self.assertEqual(self.ids(result), [1, 3, 1])
        self.assertIn('already exists', result)
        self.assertIn('Queue is empty.', result)

    def test_missing_id_and_empty_queue(self):
        result = self.run_commands('R 9\nT\nA 1 Alex 1\nR 9\nD\nQ\n')
        self.assertEqual(result.count('Error: Patient 9 not found.'), 2)
        self.assertEqual(self.ids(result), [1])

    def test_invalid_commands_do_not_mutate(self):
        invalid = ['A 2 Bob invalid', 'A 2 Bob', 'A 2 Bob 3 extra', 'T extra',
                   'R 1 extra', 'R nope', 'D extra', 'Q extra', 'UNKNOWN',
                   'A 2x Bob 3', 'A 999999999999999999999 Bob 3',
                   'A 2 Bob 999999999999999999999', 'A 2 Bob 3x']
        for command in invalid:
            with self.subTest(command=command):
                result = self.run_commands('A 1 Alex 3\n' + command + '\nD\nQ\n')
                self.assertIn('Error:', result)
                self.assertEqual(self.ids(result), [1])
                self.assertNotIn('Patient 2 Added.', result)

    def test_value_validation(self):
        for command in ['A 0 Alex 3', 'A -1 Alex 3', 'A 1 Alex 0',
                        'A 1 Alex 6', 'A 1 Alex -999', 'R 0', 'R -1']:
            with self.subTest(command=command):
                result = self.run_commands(command + '\nD\nQ\n')
                self.assertIn('Error:', result)
                self.assertEqual(self.ids(result), [])

    def test_name_boundaries(self):
        for size, accepted in [(100, True), (101, False)]:
            with self.subTest(size=size):
                result = self.run_commands(f'A 1 {"x" * size} 3\nD\nQ\n')
                self.assertEqual(self.ids(result), [1] if accepted else [])

    def test_overlong_line_discard_and_recovery(self):
        result = self.run_commands('A 1 ' + 'x' * 700 + ' 3\nA 2 Blair 5\nD\nQ\n')
        self.assertIn('Command is too long', result)
        self.assertEqual(self.ids(result), [2])

    def test_eof_blank_lines_and_final_line_without_newline(self):
        self.assertEqual(self.run_commands(''), '')
        self.assertEqual(self.run_commands('\n \t\n'), '')
        self.assertEqual(self.ids(self.run_commands('A 1 Alex 3\nD')), [1])

    def test_allocation_failures_and_cleanup(self):
        result = subprocess.run([str(MEMORY)], capture_output=True, text=True, timeout=3)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout.count('Could not allocate memory'), 3)


if __name__ == '__main__':
    unittest.main(verbosity=2)

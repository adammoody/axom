#!/usr/bin/env python
#
# Run tests for shroud
# run input file, compare results to reference files
#
# Directory structure
#  src-dir/name.yaml
#  src-dir/ref/name/      reference results
#
#  bin-dir/test/name/     generated files

# logging.debug('This is a debug message')
# logging.info('This is an info message')
# logging.warning('This is a warning message')
# logging.error('This is an error message')
# logging.critical('This is a critical error message')

from __future__ import print_function

import argparse
import filecmp
import logging
import os
import subprocess
import sys

#subprocess.call("ls -l", shell=True)

#proc = subprocess.Popen(['tail', '-500', 'mylogfile.log'], stdout=subprocess.PIPE)
#for line in proc.stdout.readlines():
#    print line.rstrip()


class Tester:
    def __init__(self):
        self.test_input_dir = ''
        self.test_output_dir = ''

        self.code_path = ''

        self.testyaml = ''   # input file
        self.ref_dir = ''    # reference directory
        self.result_dir = ''

    def set_environment(self, input, output, executable=None):
        self.test_input_dir = input
        self.test_output_dir = output

        if not os.path.isdir(output):
            raise SystemExit('Missing binary directory: ' + output)
        if not os.path.isdir(input):
            raise SystemExit('Missing source directory: ' + input)
        if executable:
            if not os.path.isdir(executable):
                raise SystemExit('Missing executable directory: ' +
                                 executable)
            self.code_path = os.path.join(executable, 'shroud')
            logging.info('Code to test: ' + self.code_path)
                

    def set_test(self, name, replace_ref=False):
        logging.info('--------------------------------------------------')
        logging.info('Testing ' + name)

        self.testyaml = os.path.join(self.test_input_dir, name + '.yaml')
        logging.info('Input file: ' + self.testyaml)
        if not os.path.isfile(self.testyaml):
            logging.error('Input file does not exist')
            return False

        self.ref_dir = os.path.join(self.test_input_dir, name)
        logging.info('Reference directory: ' + self.ref_dir)

        if replace_ref:
            # replacing reference, just create directly in ref directory
            self.result_dir = self.ref_dir
        else:
            self.result_dir = os.path.join(self.test_output_dir, name)
            logging.info('Result directory: ' + self.result_dir)
            makedirs(self.result_dir)
            clear_files(self.result_dir)

        return True

    def do_module(self, name):
        pass

    def do_test(self):
        """ Run test, return True/False for pass/fail.
        Files must compare, with no extra or missing files.
        """
        cmd = [
            self.code_path,
            '--path', self.test_input_dir,
            '--logdir', self.result_dir,
            '--outdir', self.result_dir,
            self.testyaml,
            ]
        logging.debug(' '.join(cmd))

        try:
            output = subprocess.check_output(
                cmd,
                stderr=subprocess.STDOUT,
                universal_newlines=True)
        except subprocess.CalledProcessError as exc:
            logging.error('Exit status: %d' % exc.returncode)
            logging.error(exc.output)
            return False

        output_file = os.path.join(self.result_dir, 'output')
        fp = open(output_file, 'w')
        fp.write(output)
        fp.close()

        return True

    def do_compare(self):
        status = True  # assume it passes

        cmp = filecmp.dircmp(self.ref_dir, self.result_dir)
        if not os.path.exists(self.ref_dir):
            logging.info('Reference directory does not exist: ' + self.ref_dir)
            return False

        match, mismatch, errors = filecmp.cmpfiles(self.ref_dir, self.result_dir, cmp.common)
        for file in cmp.common:
            logging.info('Compare: ' + file)
        if mismatch:
            status = False
            for file in mismatch:
                logging.warn('Does not compare: '+ file)
        if errors:
            status = False
            for file in errors:
                logging.warn('Unable to compare: ' + file)

        if cmp.left_only:
            status = False
            for file in cmp.left_only:
                logging.warn('Only in reference: ' + file)
        if cmp.right_only:
            status = False
            for file in cmp.right_only:
                logging.warn('Only in result: ' + file)

        if status:
            logging.info('Test {} pass'.format(name))
        else:
            logging.info('Test {} fail'.format(name))
        return status


def makedirs(path):
    """ Make sure directory exists.
    """
    try: 
        os.makedirs(path)
    except OSError:
        if not os.path.isdir(path):
            raise
    # os.makedirs(path,exist_ok=True) python3  3.2


def clear_files(path):
    """Remove all files in a directory.
    """
    for file in os.listdir(path):
        full_path = os.path.join(path, file)
        try:
            if os.path.isfile(full_path):
                os.unlink(full_path)
        except Exception, e:
            logging.warning('Unable to remove file: ' + full_path)
            logging.warning(e)


if __name__ == '__main__':
    # XXX raise KeyError(key)

    parser = argparse.ArgumentParser(prog='do-test')
    parser.add_argument('-r', action='store_true',
                        help='Replace test results')
    parser.add_argument('testname', nargs='*',
                        help='test to run')
    args = parser.parse_args()

    replace_ref = args.r

    # XXX - get directories from environment or command line options

    tester = Tester()

    tester.set_environment(os.environ['TEST_INPUT_DIR'], 
                           os.environ['TEST_OUTPUT_DIR'],
                           os.environ['EXECUTABLE_DIR'])

    logname = 'test.log'
    logging.basicConfig(filename=os.path.join(
        tester.test_output_dir, logname),
                        filemode='w',
                        level=logging.DEBUG,
    )

    if args.testname:
        test_names = args.testname
    else:
        test_names = [ 'tutorial', 'example', 'include', 'names', 'strings' ]

    logging.info('Tests to run: {}'.format( ' '.join(test_names)))

    pass_names = []
    fail_names = []
    for name in test_names:
        status = tester.set_test(name, replace_ref)

        if status:
            status = tester.do_test()

            if status and not replace_ref:
                status = tester.do_compare()

        if status:
            pass_names.append(name)
            print('{} pass'.format(name))
        else:
            fail_names.append(name)
            print('{} failed'.format(name))

    # summarize results
    if fail_names:
        exit_status = 1
        msg = "Not all tests passed"
    else:
        exit_status = 0
        msg = "All tests passed"
    print(msg)
    logging.info(msg)

    logging.shutdown()
    sys.exit(exit_status)

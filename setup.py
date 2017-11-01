import sys
from setuptools import setup, Extension
from setuptools.command.test import test as TestCommand

CLASSIFIERS = [
    'Development Status :: 3 - Alpha',
    'Intended Audience :: Developers',
    'License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 3.4',
    'Programming Language :: Python :: 3.5',
    'Programming Language :: Python :: 3.6',
    'Topic :: Software Development :: Embedded Systems',
    'Topic :: System :: Emulators',
]

LONG_DESCRIPTION = open('README.md', 'rt').read()

pin_module = Extension('icemu._pin', sources=['modules/pin.c'])

class PyTest(TestCommand):
    def run_tests(self):
        import pytest
        errno = pytest.main()
        sys.exit(errno)

setup(
    #...,
    )
setup(
    name='icemu',
    version='0.4.0',
    author='Virgil Dupras',
    author_email='hsoft@hardcoded.net',
    packages=['icemu'],
    scripts=[],
    ext_modules=[pin_module],
    tests_require=['pytest'],
    url='https://github.com/hsoft/icemu',
    license='LGPLv3',
    description='Emulate Integrated Circuits',
    long_description=LONG_DESCRIPTION,
    classifiers=CLASSIFIERS,
    cmdclass = {'test': PyTest},
)


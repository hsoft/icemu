from setuptools import setup

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

setup(
    name='icemu',
    version='0.3.0',
    author='Virgil Dupras',
    author_email='hsoft@hardcoded.net',
    packages=['icemu'],
    scripts=[],
    url='https://github.com/hsoft/icemu',
    license='LGPLv3',
    description='Emulate Integrated Circuits',
    long_description=LONG_DESCRIPTION,
    classifiers=CLASSIFIERS,
)


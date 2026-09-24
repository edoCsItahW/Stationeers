"""Package the prebuilt IC10 compiler Python extension.

`ic10c_python` is not a Python source module: it is a native extension compiled by the C++
build (see ``code/scripts/buildIC10CompilerPython.*``) that lands in this directory as
``ic10c_python.pyd`` (Windows) or ``ic10c_python.so`` (Linux), together with its
``ic10c_python.pyi`` type stub.

setuptools' ``build_py`` only picks up ``<module>.py`` sources, so ``py-modules`` alone
would produce an empty wheel. This extends ``build_py`` to ship the prebuilt binary and
the stub instead of a source file.
"""

import glob
import os

from setuptools import setup
from setuptools.command.build_py import build_py as _build_py

NATIVE_PATTERNS = ("ic10c_python.pyd", "ic10c_python.so", "ic10c_python.*.pyd", "ic10c_python.*.so")
STUB_NAME = "ic10c_python.pyi"


def _payload():
    """Absolute paths of the files to ship, in a stable order."""
    files = []
    for pattern in NATIVE_PATTERNS:
        files.extend(sorted(glob.glob(pattern)))
    if os.path.isfile(STUB_NAME):
        files.append(STUB_NAME)
    return files


class build_py(_build_py):
    """Ship the CMake-built extension and its stub; there is no .py source."""

    def find_modules(self):
        return []

    def run(self):
        super().run()
        for path in _payload():
            destination = os.path.join(self.build_lib, os.path.basename(path))
            self.mkpath(os.path.dirname(destination))
            self.copy_file(path, destination)

    def get_outputs(self, include_bytecode=True):
        outputs = super().get_outputs(include_bytecode)
        outputs.extend(
            os.path.join(self.build_lib, os.path.basename(path)) for path in _payload()
        )
        return outputs


setup(cmdclass={"build_py": build_py})

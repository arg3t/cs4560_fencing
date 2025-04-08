import lit.formats
import os

config.name = "FencingTests"

config.test_format = lit.formats.ShTest(True)

config.suffixes = [".ll"]

config.test_source_root = config.test_exec_root = os.path.dirname(__file__)

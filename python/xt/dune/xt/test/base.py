# ~~~
# This file is part of the dune-xt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
# Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2020)
#   René Fritze     (2018 - 2019)
#   Tobias Leibner  (2019 - 2020)
# ~~~

import logging
import pkgutil
import pprint


def load_all_submodule(module):
    ignore_playground = True
    fails = []
    for _, module_name, _ in pkgutil.walk_packages(
        module.__path__, module.__name__ + ".", lambda n: fails.append((n, ""))
    ):
        if ignore_playground and "playground" in module_name:
            continue
        try:
            __import__(module_name, level=0)
        except (TypeError, ImportError) as t:
            fails.append((module_name, t))
    if len(fails) > 0:
        logging.getLogger(module.__name__).fatal(
            f"Failed imports: {pprint.pformat(fails)}"
        )
        raise ImportError(module.__name__)


def runmodule(filename):
    import sys

    import pytest

    sys.exit(pytest.main(sys.argv[1:] + [filename]))

#!/bin/sh

#  Copyright 2013 Eric Niebler
#
#  Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

chmod -R a+r html
rsync --delete --rsh=ssh --recursive -p html/* eric_niebler@frs.sourceforge.net:/home/user-web/eric_niebler/htdocs/libs/quick_check/doc/html

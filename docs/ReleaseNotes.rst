=====================================
Clang 3.8 (In-Progress) Release Notes
=====================================

.. contents::
   :local:
   :depth: 2

Written by the `LLVM Team <http://llvm.org/>`_

.. warning::

   These are in-progress notes for the upcoming Clang 3.8 release. You may
   prefer the `Clang 3.6 Release Notes
   <http://llvm.org/releases/3.6.0/tools/clang/docs/ReleaseNotes.html>`_.

Introduction
============

This document contains the release notes for the Clang C/C++/Objective-C
frontend, part of the LLVM Compiler Infrastructure, release 3.8. Here we
describe the status of Clang in some detail, including major
improvements from the previous release and new feature work. For the
general LLVM release notes, see `the LLVM
documentation <http://llvm.org/docs/ReleaseNotes.html>`_. All LLVM
releases may be downloaded from the `LLVM releases web
site <http://llvm.org/releases/>`_.

For more information about Clang or LLVM, including information about the
latest release, please check out the main `Clang Web Site
<http://clang.llvm.org>`_ or the `LLVM Web Site <http://llvm.org>`_.

Note that if you are reading this file from a Subversion checkout or the main
Clang web page, this document applies to the *next* release, not the current
one. To see the release notes for a specific release, please see the `releases
page <http://llvm.org/releases/>`_.

What's New in Clang 3.8?
========================

Some of the major new features and improvements to Clang are listed here.
Generic improvements to Clang as a whole or to its underlying infrastructure
are described first, followed by language-specific sections with improvements
to Clang's support for those languages.

Major New Features
------------------

- Feature1...

Improvements to Clang's diagnostics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clang's diagnostics are constantly being improved to catch more issues,
explain them more clearly, and provide more accurate source information
about them. The improvements since the 3.5 release include:

-  ...

New Compiler Flags
------------------

The option ....


New Pragmas in Clang
-----------------------

Clang now supports the ...

Windows Support
---------------

Clang's support for building native Windows programs ...


C Language Changes in Clang
---------------------------

...

C11 Feature Support
^^^^^^^^^^^^^^^^^^^

...

C++ Language Changes in Clang
-----------------------------

- ...

C++1y Feature Support
^^^^^^^^^^^^^^^^^^^^^

Clang 3.4 supports all the features in the current working draft of the
upcoming C++ standard, provisionally named C++1y. Support for the following
major new features has been added since Clang 3.3:

- Generic lambdas and initialized lambda captures.
- Deduced function return types (``auto f() { return 0; }``).
- Generalized ``constexpr`` support (variable mutation and loops).
- Variable templates and static data member templates.
- Use of ``'`` as a digit separator in numeric literals.
- Support for sized ``::operator delete`` functions.

In addition, ``[[deprecated]]`` is now accepted as a synonym for Clang's
existing ``deprecated`` attribute.

Use ``-std=c++1y`` to enable C++1y mode.

OpenCL C Language Changes in Clang
----------------------------------

...

Internal API Changes
--------------------

These are major API changes that have happened since the 3.7 release of
Clang. If upgrading an external codebase that uses Clang as a library,
this section should help get you past the largest hurdles of upgrading.

-  ...

libclang
--------

...

Static Analyzer
---------------

...

Clang Format
------------

Clang now includes a new tool ``clang-format`` which can be used to
automatically format C, C++ and Objective-C source code. ``clang-format``
automatically chooses linebreaks and indentation and can be easily integrated
into editors, IDEs and version control systems. It supports several pre-defined
styles as well as precise style control using a multitude of formatting
options. ``clang-format`` itself is just a thin wrapper around a library which
can also be used directly from code refactoring and code translation tools.
More information can be found on `Clang Format's
site <http://clang.llvm.org/docs/ClangFormat.html>`_.

Windows Support
---------------

- `clang-cl <UsersManual.html#clang-cl>`_ provides a new driver mode that is
  designed for compatibility with Visual Studio's compiler, cl.exe. This driver
  mode makes Clang accept the same kind of command-line options as cl.exe. The
  installer will attempt to expose clang-cl in any Visual Studio installations
  on the system as a Platform Toolset, e.g. "LLVM-vs2012". clang-cl targets the
  Microsoft ABI by default. Please note that this driver mode and compatibility
  with the MS ABI is highly experimental.

Python Binding Changes
----------------------

The following methods have been added:

Significant Known Problems
==========================

Additional Information
======================

A wide variety of additional information is available on the `Clang web
page <http://clang.llvm.org/>`_. The web page contains versions of the
API documentation which are up-to-date with the Subversion revision of
the source code. You can access versions of these documents specific to
this release by going into the "``clang/docs/``" directory in the Clang
tree.

If you have any questions or comments about Clang, please feel free to
contact us via the `mailing
list <http://lists.llvm.org/mailman/listinfo/cfe-dev>`_.

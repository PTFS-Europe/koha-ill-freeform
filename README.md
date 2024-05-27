# :warning: 24.05.x Koha onwards :warning:

Please utilize [branch 24.05.x](https://github.com/PTFS-Europe/koha-ill-freeform/tree/24.05.x) for Koha versions 24.05 onwards.

This is a temporary compatiblility measure while [Bug 35570](https://bugs.koha-community.org/bugzilla3/show_bug.cgi?id=35570) is still in community QA.

If [Bug 35570](https://bugs.koha-community.org/bugzilla3/show_bug.cgi?id=35570) is pushed, this repo is made obsolete.

If [Bug 35570](https://bugs.koha-community.org/bugzilla3/show_bug.cgi?id=35570) is not pushed, this repo should be turned into a Koha plugin (see [Bug 19605](https://bugs.koha-community.org/bugzilla3/show_bug.cgi?id=19605)) instead.

# Koha Interlibrary Loans FreeForm backend

This backend provides an open and flexible method to create Interlibrary Loan requests that are not tied to a specific service.

## Getting Started

The version of the backend you require depends on the version of Koha you are using:
* 17.11 - Use the 17.11 branch if you are using this version of Koha
* 18.05 - Use the 18.05 branch if you are using this version of Koha
* master - Use this branch for 18.11 and higher

## Installing

* Create a directory in `Koha` called `Illbackends`, so you will end up with `Koha/Illbackends`
* Clone the repository into this directory, so you will end up with `Koha/Illbackends/koha-ill-freeform`
* In the `koha-ill-freeform` directory switch to the branch you wish to use
* Rename the `koha-ill-freeform` directory to `FreeForm`
* Activate ILL by enabling the `ILLModule` system preference

# Koha Interlibrary Loans FreeForm backend

This backend provides an open and flexible method to create Interlibrary Loan requests that are not tied to a specific service.

## Getting Started

The version of the backend you require depends on the version of Koha you are using:
* 17.11 - Use the 17.11 branch if you are using this version of Koha
* 18.05 - Use the 18.05 branch if you are using this version of Koha
* master - We recommend against using this branch as it contains additions that are tied to pending bugs in Koha and, as such, will not work with any released version of Koha

## Installing

* Create a directory in `Koha` called `Illbackends`, so you will end up with `Koha/Illbackends`
* Clone the repository into this directory, so you will end up with `Koha/Illbackends/FreeForm`
* In the `FreeForm` directory switch to the branch you wish to use
* Activate ILL by enabling the `ILLModule` system preference

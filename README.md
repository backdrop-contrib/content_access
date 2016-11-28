#Content Access Module

Yet another node access module.
This module allows you to manage permissions for content types by role. It allows you to specifiy
custom view, view own, edit, edit own, delete and delete own permissions for each content type.
Optionally you can enable per content node access settings, so you can customize the access for each
content node.

In particular:

  * it comes with sensible defaults, so you need not configure anything and everything stays working
  * it is as flexible as you want. It can work with per content type settings, per content node settings
    as well as with flexible Access Control Lists (with the help of the [ACL module](https://backdropcms.org/project/acl)).
  * it tries to reuse existing functionality instead of reimplementing it. So one can install the ACL
    module and set per user access control settings per content node.
    Furthermore the module provides conditions and actions for the Rules module, which allows one
    to configure even rule-based access permissions.
  * it optimizes the written content node grants, so that only the really necessary grants are written.
    This is important for the performance of your site.
  * it treats access control as importantly as it should. (e.g. the module has a bunch of simpletests to ensure that everything is working right.)
  * it respects and makes use of Backdrop's built in permissions as far as 
    possible. This means the access control tab provided by this module takes 
    these permissions into account and provides you a good overview about the 
    access control settings that will be applied. [1]

So the module is simple to use, but can be configured to provide really 
fine-grained permissions!


##Installation

 * Install this module using the official 
  [Backdrop CMS instructions](https://backdropcms.org/guide/modules)
 * Optionally download and install the [ACL module](https://backdropcms.org/project/acl) too.
 * Edit a content type at `admin/structure/types`. There will be a new tab 
   called "Access Control".


##[ACL Module](https://backdropcms.org/project/acl)

To make use of `Access Control Lists` you'll need to enable per content node 
access control settings for a content type. At the access control tab of such a
content node you are able to grant view, edit or delete permission for specific
users.


##Running multiple node access modules on a site (Advanced!)

A Backdrop node access module can only grant access to content nodes, but not deny it. So if you
are using multiple node access modules, access will be granted to a node as soon as one of the
module grants access to it.

However you can influence the behaviour by changing the priority of the content access module as
Backdrop applies *only* the grants with the highest priority. So if `Content Access` has the highest
priority *alone*, only its grants will be applied. 

By default node access modules use priority 0.

##LICENSE

This project is GPL v2 software. See the LICENSE.txt file in this directory 
for complete text.

##CURRENT MAINTAINERS

Looking for maintainers

##CREDITS   

 - This module was ported to Backdrop by [Biolithic](https://github.com/biolithic)
and [Docwilmot](https://github.com/docwilmot).
 - This module is a fork of the Drupal version by [Wolfgang Ziegler](nuppla@zites.net)

##Footnotes

[1] Note that this overview can't take other modules into account, which might also alter node access.
    If you have multiple modules installed that alter node access, read the paragraph about "Running 
    multiple node access modules on a site".

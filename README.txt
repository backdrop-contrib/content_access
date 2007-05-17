$Id$

Content Access Module
-----------------------
by Wolfgang Ziegler, nuppla@zites.net

Yet another node access module.
So this module allows you to manage permissions for content types by role and author. It allows
you to specifiy custom view, edit and delete permissions for each content type. Optionally you
can also enable per node access settings, so you can customize the access for each node.

In particular
  * it comes with sensible defaults, so you need not configure anything and everything stays working
  * it is as flexible as you want. It can work with per content type settings, per node settings
    as well as with flexible Access Control Lists.
  * it trys to reuse existing functionality instead of reimplementing it. So one can install the ACL
    module and set per user access control settings per node.
  * it optimizes the written node grants, so that only really necessary grants are written.

So the module is simple to use, but can be configured to provide really fine-grained permissions!


Installation
------------
 * Copy the content access module's directory to your modules directory and activate the module.
 * Optionally download and install the ACL module too.
 * Edit a content type at admin/content/types. There will be a new tab "Access Control".


ACL Module
-----------
You can find the ACL module at http://drupal.org/project/acl. Note that you'll need to install at
least version 1.4 of the module. To make use of Access Control Lists you'll need to enable per node
access control settings for a content type. At the access control tab of such a node you'll be
able to grant single users view, edit or delete permission for this node.


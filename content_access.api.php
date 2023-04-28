<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Respond to the creation of an ACL. This hook is invoked after ACL module saves
 * the information to the database.
 *
 * @param $node
 *   The node to which the ACL applies.
 * @param string $op
 *   One of 'view', 'update', 'delete'
 * @param array $uids_before
 *   The uids on the ACL for this $op, before the current operation
 * @param array $uids
 *   The uids on the ACL for this $op, as saved.
 */
function hook_user_acl($node, $op, $uids_before, $uids) {
  
}

/**
 * Respond to the application of per-node content access rules.
 *
 * @param $node
 *   The node to which the rules apply.
 * @param array $settings
 *   The keyed array saved by content_access_save_per_node_settings()
 */
function hook_per_node($node, $settings) {
  
}

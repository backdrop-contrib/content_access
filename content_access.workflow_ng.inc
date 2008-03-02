<?php

/**
 * @file
 * Workflow-ng specific functions that expose content_access's API.
 */

/**
 * Parse submitted settings for per-node form into internal format.
 * 
 * @param $form_values
 *   Form values array variable, modified settings are also reflected here.
 */
function content_access_parse_settings(&$form_values) {
  $settings = array();
  foreach (array('view', 'update', 'delete') as $setting) {
    if (is_array($form_values[$setting])) {
      unset($form_values[$setting][0]);
      $form_values[$setting] = array_filter($form_values[$setting]);
    }
    $settings[$setting] = $form_values[$setting];
  }
  return $settings;
}

/**
 * Implementation of hook_action_info().
 */
function content_access_action_info() {
  return array(
    'content_access_action_set_node_permissions' => array(
      '#label' => t('Set content permissions'),
      '#arguments' => array(
        'node' => array('#entity' => 'node', '#label' => t('Content')),
      ),
      '#module' => 'Content access',
    ),
    'content_access_action_reset_node_permissions' => array(
      '#label' => t('Reset content permissions'),
      '#arguments' => array(
        'node' => array('#entity' => 'node', '#label' => t('Content')),
      ),
      '#module' => 'Content access',
    ),
  );
}

/**
 * Action implementation: Set permissions for a node.
 */
function content_access_action_set_node_permissions($node, $settings) {
  if (!content_access_get_settings('per_node', $node->type)) {
    workflow_ng_handle_error_msg('Cannot set per-node permissions for '. $node->type);
    return;
  }
  content_access_save_per_node_settings($node, $settings);
  return array('node' => $node);
}

/**
 * Form for configuring content_access_action_set_node_permissions action.
 */
function content_access_action_set_node_permissions_form($settings = array(), $argument_info) {
  return content_access_page_form($settings);
}

/**
 * Save configuration for content_access_action_set_node_permissions action.
 */
function content_access_action_set_node_permissions_submit($form_id, $form_values) {
  return content_access_parse_settings($form_values);
}

/**
 * Action implementation: Reset permissions for a node.
 */
function content_access_action_reset_node_permissions($node) {
  if (!content_access_get_settings('per_node', $node->type)) {
    workflow_ng_handle_error_msg('Cannot set per-node permissions for '. $node->type);
    return;
  }
  $settings = array();
  foreach (array('view', 'update', 'delete') as $op) {
    $settings[$op] = content_access_get_settings($op, $node->type);
  }
  content_access_save_per_node_settings($node, $settings);
  return array('node' => $node);
}

/**
 * Implementation of hook_condition_info().
 */
function content_access_condition_info() {
  return array(
    'content_access_condition_check_permissions' => array(
    '#label' => t('Node permissions'),
    '#arguments' => array(
      'node' => array('#entity' => 'node', '#label' => t('Content')),
    ),
    '#description' => t('Evaluates to TRUE, if content access allows all selected operations for the given roles.'),
    '#module' => 'Content access',
    ),
  );
}

/**
 * Condition implementation: Check if node has permissions.
 * 
 * @note
 *   This will only check for the existence of permissions, not the
 *   absence of. I.e. a rule that checks just for authenticated write
 *   will return TRUE for a node that allows authenticated and anonymous
 *   write.
 */
function content_access_condition_check_permissions($node, $settings) {
   if (!content_access_get_settings('per_node', $node->type)) {
    workflow_ng_handle_error_msg('Cannot check per-node permissions for '. $node->type);
    return FALSE;
  }
  // Compare our settings with node's settings.
  foreach (array('view', 'update', 'delete') as $op) {
    $expect_roles = $settings[$op];
    $current_roles = content_access_per_node_setting($op, $node);
    foreach ($expect_roles as $role) {
      if (empty($current_roles[$role])) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Form for configuring content_access_condition_check_permissions().
 */
function content_access_condition_check_permissions_form($settings = array()) {
  return content_access_page_form($settings);
}

/**
 * Save configuration for content_access_condition_check_permissions().
 */
function content_access_condition_check_permissions_submit($form_id, $form_values) {
  return content_access_parse_settings($form_values);
}

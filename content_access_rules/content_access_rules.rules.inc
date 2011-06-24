<?php

/**
 * @file
 * Rules specific functions that expose content_access' API.
 *
 * @todo
 * A way to enable per-node settings when a rule created, otherwise no effects
 *   will be noticed.
 * Clean-up function names.
 */

/**
 * Implements hook_rules_event_info().
 *
 * @ingroup rules
 */
function content_access_rules_rules_event_info() {
  $events['content_access_content_type'] = array('label' => t('Content type access changed'));

  if (module_exists('acl')) {
    $events['content_access_content_user'] = array('label' => t('Grant user on content'));
  }

  $items = array();
  foreach ($events as $name => $event) {
    $items[$name] = array(
      'label' => $event['label'],
      'group' => t('Content Access'),
    );
  }

  return $items;
}

/**
 * Implementation of hook_rules_action_info().
 *
 * @ingroup rules
 */
function content_access_rules_rules_action_info() {
  $role_actions = array(
    'content_access_action_grant_node_permissions' => array(
      'label' => t('Grant content permissions by role'),
      'description' => t('Grant node permissions to specific role(s).'),
    ),
    'content_access_action_revoke_node_permissions' => array(
      'label' => t('Revoke content permissions by role'),
      'description' => t('Revoke node permissions from specific role(s).'),
    ),
    'content_access_action_reset_node_permissions' => array(
      'label' => t('Reset content permissions'),
      'description' => t('Reset node permissions to default permissions.'),
    ),
  );

  $user_actions = array(
    'content_access_action_user_grant_view' => array(
      'label' => t('Grant user view access'),
      'description' => t('Grant node view to specific user(s).'),
    ),
    'content_access_action_user_grant_update' => array(
      'label' => t('Grant user update access'),
      'description' => t('Grant node update to specific user(s).'),
    ),
    'content_access_action_user_grant_delete' => array(
      'label' => t('Grant user delete access'),
      'description' => t('Grant node delete to specific user(s).'),
    ),
  );

  $items = array();
  foreach ($role_actions as $name => $action) {
    $items[$name] = array(
      'label' => $action['label'],
      'named parameter' => TRUE,
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'description' => $action['description'],
        ),
        'content_access' => array(
          'type' => 'list<text>',
          'label' => t('Role Based Access Control Settings'),
          'optional' => TRUE,
          'options list' => 'content_access_action_roles_permissions_list',
          'restriction' => 'input',
        ),
      ),
      'group' => t('Content Access Role'),
      'callbacks' => array(
        'form_alter' => 'content_access_action_form_alter',
      ),
    );
  }

  if (module_exists('acl')) {
    foreach ($user_actions as $name => $action) {
      $items[$name] = array(
        'label' => $action['label'],
        'named parameter' => TRUE,
        'parameter' => array(
          'node' => array(
            'type' => 'node',
            'label' => t('Content'),
            'description' => $action['description'],
          ),
          'content_access' => array(
            'type' => 'user',
            'label' => t('User Based Access Control Settings'),
            'description' => t('User to grant permissions.'),
          ),
        ),
        'group' => t('Content Access User'),
      );
    }
  }

  return $items;
}

/**
 * Returns an array of operations used by the module.
 */
function content_access_action_roles_permissions_list() {
  return _content_access_roles_list();
}

/**
 * Helper function to return the array of available access operations.
 */
function _content_access_roles_list() {
  $defaults = array();
  $roles = array_map('filter_xss_admin', user_roles());

  // Make sure defaults are set properly
  foreach (_content_access_get_operations() as $op => $label) {
    foreach ($roles as $rid => $role) {
      $defaults[$op][$op . ':' . $role] = $role;
    }
  }

  return $defaults;
}

/**
 * Alter the settings form to render the text<list> as checkboxes, and to add
 * a submit callback to the form submit button.
 */
function content_access_action_form_alter(&$form, &$form_state) {
  // Alter the text<list> to make it into checkboxes groups
  $ca_elements =& $form['parameter']['content_access']['settings']['content_access'];
  $ca_elements = content_access_list_to_checkboxes($ca_elements);

  // Check and disable default roles.
  foreach ($ca_elements as $op => $select) {
    $ca_elements[$op]['#process'] = array('form_process_checkboxes', 'content_access_rules_disable_checkboxes');
  }

  // Add submit callback to process content_access_rules_role_based_form()
  // rules_ui_edit_element_submit is the default submit callback so we add it
  // again.
  // @todo I think this should go into Rules just like in FAPI
  $form['submit']['#submit'] = array('content_access_action_settings_form_submit', 'rules_ui_edit_element_submit');
}

/**
 * Settings form submit handler.
 *
 * Get the form settings as checkboxes, convert them back to list<text> so
 * Rules can understand and process them.
 */
function content_access_action_settings_form_submit($form, &$form_state) {
  $ca_element =& $form_state['input']['parameter']['content_access']['settings']['content_access'];
  $ca_element = content_access_checkboxes_to_list_submit($ca_element);

  $form_state['rules_element']->settings['content_access'] = $ca_element;
}

/**
 * Desc.
 */
function content_access_list_to_checkboxes($element) {
  $checkboxes = array();
  $operations = _content_access_get_operations();

  foreach ($operations as $op => $label) {
    $checkboxes[$op] = $element;
    $checkboxes[$op]['#title'] = t($label);
    $checkboxes[$op]['#type'] = 'checkboxes';
    $checkboxes[$op]['#options'] = $element['#options'][$op];
    unset($checkboxes[$op]['#default_value']);

    // Set defaults values of checkboxes
    foreach ($checkboxes[$op]['#options'] as $op_role => $permission) {
      if (isset($element['#default_value'][$op_role])) {
        $checkboxes[$op]['#default_value'][] = $op_role;
      }
    }
  }

  return $checkboxes;
}

/**
 * Desc.
 */
function content_access_checkboxes_to_list($element) {
  $list = $element[key($element)];
  $list['#title'] = 'Value';
  $list['#type'] = 'select';
  unset($list['#options']);

  foreach ($element as $op => $value) {
    $list['#options'][$op] = $value['#options'];
  }
  return $list;
}

/**
 * Desc.
 */
function content_access_checkboxes_to_list_submit($element) {
  $list = array();
  foreach (_content_access_roles_list() as $op => $op_roles) {
    foreach ($op_roles as $op_role => $permission) {
      if (isset($element[$op][$op_role]) && $element[$op][$op_role]) {
        $list[$op_role] = $permission;
      }
    }
  }
  return $list;
}

/**
 * Action implementation: Grant permissions for a node.
 */
function content_access_action_grant_node_permissions($params) {
  // @todo what if content is not node?
  if (_content_access_rules_check_setting($params['node'])) {
    $settings = content_access_action_settings($params['content_access']);
    content_access_save_per_node_settings($params['node'], $settings);
    // @todo find a better way to check for node save?
    if (!content_access_rules_check_grants($params['node']->nid) && $params['node']->op != 'Save') {
      node_access_acquire_grants($params['node']);
    }

    // A following node_save() updates the grants for us.
    return array('node' => $params['node']);
  }
}

/**
 * Action implementation: Revoke permissions for a node.
 */
function content_access_action_revoke_node_permissions($params) {
  if (_content_access_rules_check_setting($params['node'])) {
    $settings = content_access_action_settings($params['content_access']);
    content_access_save_per_node_settings($params['node'], $settings);
    if (!content_access_rules_check_grants($params['node']->nid) && $params['node']->op != 'Save') {
      node_access_acquire_grants($params['node']);
    }

    // A following node_save() updates the grants for us.
    return array('node' => $params['node']);
  }
}

/**
 * Action implementation: Reset permissions for a node.
 */
function content_access_action_reset_node_permissions($params) {
  if (_content_access_rules_check_setting($params['node'])) {
    content_access_delete_per_node_settings($params['node']);

    // A following node_save() updates the grants for us.
    return array('node' => $params['node']);
  }
}

/**
 * Verifies that per content settings are activated for the given node.
 */
function _content_access_rules_check_setting($node) {
  $type = $node->type;
  $settings = variable_get('content_access_settings', array());

  foreach ($settings as $setting) {
    if (isset($setting[$type]) && $setting[$type]) {
      return TRUE;
    }
  }

  // If we didn't find any settings in content access for this type return
  // false as we don't want to process it.
  rules_log(t("Can't set per content permissions for content type @type. Make sure to have per content settings activated for content types you want to alter access control for.", array('@type' => $node->type)), TRUE);
  return FALSE;
}

/**
 * Split the settings string into array.
 */
function content_access_action_settings($param) {
  $roles_ids = array_flip(user_roles());

  foreach (_content_access_get_operations() as $op => $label) {
    $settings[$op] = array();
  }

  foreach ($param as $op_role => $role) {
    $op = substr($op_role, 0, strpos($op_role, ':'));
    $rid = $roles_ids[$role];
    $settings[$op][] = $rid;
  }

  return $settings;
}

/**
 * Check if a given nid has grants in {node_access} or not.
 */
function content_access_rules_check_grants($nid) {
  return db_select('node_access')
          ->condition('nid', $nid)
          ->countQuery()->execute()->fetchField();
}

/**
 * Action implementation: Grant user view access.
 */
function content_access_action_user_grant_view($params) {
  content_access_action_user_grant($params['node'], $params['content_access']->uid, 'view');
}

/**
 * Action implementation: Grant user update access.
 */
function content_access_action_user_grant_update($params) {
  content_access_action_user_grant($params['node'], $params['content_access']->uid, 'update');
}

/**
 * Action implementation: Grant user delete access.
 */
function content_access_action_user_grant_delete($params) {
  content_access_action_user_grant($params['node'], $params['content_access']->uid, 'delete');
}

/**
 * Process Rule's param, and grant by the passed operation.
 */
function content_access_action_user_grant($node, $uid, $op) {
  module_load_include('inc', 'content_access', 'content_access.admin');

  $acl_id = content_access_get_acl_id($node, $op);
  acl_node_add_acl($node->nid, $acl_id, (int) ($op == 'view'), (int) ($op == 'update'), (int) ($op == 'delete'), content_access_get_settings('priority', $node->type));
  acl_add_user($acl_id, $uid);

  node_access_acquire_grants($node);
}
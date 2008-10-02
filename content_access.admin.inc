<?php
// $Id$

/**
 * @file Content access administration UI.
 */

/**
 * Per node settings page.
 */
function content_access_page(&$form_state, $node) {
  drupal_set_title(t('Access control for %title', array('%title' => $node->title)));

  foreach (array('view', 'update', 'delete') as $i) {
    $defaults[$i] = content_access_per_node_setting($i, $node);
  }

  $form = content_access_page_form($defaults, $node);

  $form_state['node'] = $node;
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );
  return $form;
}

/**
 * Builds per node setting page form without requiring a node. Used by
 * content_access_action_set_node_permissions_form().
 *
 * @param $defaults
 *   Array of defaults for view/update/delete checkboxes.
 * @param $node
 *   Optional node for ACL.
 */
function content_access_page_form($defaults = array(), $node = FALSE) {

  // Make sure defaults array is full.
  foreach (array('view', 'update', 'delete') as $op) {
    if (!isset($defaults[$op])) $defaults[$op] = array();
  }

  $roles = user_roles();
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Role access control settings'),
    '#collapsible' => TRUE,
  );

  if (!$node) {
    $form['settings']['#description'] = t('Warning: No defaults are set; be sure to fill out all boxes appropriately.');
  }

  drupal_add_css(drupal_get_path('module', 'content_access') . '/content_access.css');
  $form['settings']['view'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('View'),
    '#default_value' => $defaults['view'],
  );
  $form['settings']['update'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Edit'),
    '#default_value' => $defaults['update'],
  );
  $form['settings']['delete'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Delete'),
    '#default_value' => $defaults['delete'],
  );
  $form['settings']['clearer'] = array(
    '#value' => '<br clear="all" />',
  );
  if (module_exists('acl') && $node) {
    // This is disabled when there is no node passed.
    $form['acl'] = array(
      '#type' => 'fieldset',
      '#title' => t('User access control lists'),
      '#description' => t('These settings allow you to grant access to specific users.'),
      '#collapsible' => TRUE,
      '#tree' => TRUE,
    );
    foreach (array('view', 'update', 'delete') as $op) {
      $acl_id = acl_get_id_by_name('content_access', $op .'_'. $node->nid);
      if (!$acl_id) {
        // Create one:
        $acl_id = acl_create_new_acl('content_access', $op .'_'. $node->nid, content_access_get_settings('priority', $node->type));
        acl_node_add_acl($node->nid, $acl_id, $op == 'view', $op == 'update', $op == 'delete');
      }
      $form['acl'][$op] = acl_edit_form($acl_id, 'Grant '. $op .' access');
      $form['acl'][$op]['#collapsed'] = !isset($_POST['acl'][$op]['add_button']) && !isset($_POST['acl'][$op]['delete_button']);
    }
  }
  return $form;
}

function content_access_page_submit($form_id, &$form_state) {
  $form_values = $form_state['values'];
  $node = $form_state['node'];
  $settings = array();
  foreach (array('view', 'update', 'delete') as $op) {
    // Set the settings so that further calls will return this settings.
    unset($form_values[$op][0]);
    $settings[$op] = array_filter($form_values[$op]);

    if (module_exists('acl') && isset($form_values['acl'][$op])) {
      acl_save_form($form_values['acl'][$op]);
    }
  }

  // Save per-node settings.
  content_access_save_per_node_settings($node, $settings);

  // Apply new settings.
  node_access_acquire_grants($node);
  drupal_set_message('Your changes have been saved.');
}

/**
 * Per content type administration page form.
 */
function content_access_admin_settings(&$form_state, $type) {
  $form_state['type'] = $type;
  $roles = user_roles();
  // Per type:
  $form['type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content type access control settings'),
    '#collapsible' => TRUE,
    '#description' => t('Note that users need at leat the @access_content permission to be able to deal in any way with content.', array('@access_content' => t('access content'))),
  );
  drupal_add_css(drupal_get_path('module', 'content_access') . '/content_access.css');
  $form['type']['view'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('View any content'),
    '#default_value' => content_access_get_settings('view', $type),
  );
  $form['type']['update'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Edit any content'),
    '#default_value' => content_access_get_settings('update', $type),
  );
  $form['type']['delete'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Delete any content'),
    '#default_value' => content_access_get_settings('delete', $type),
  );
  $form['type']['clearer'] = array(
    '#value' => '<br clear="all" />',
  );
  $form['type']['view_own'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('View own content'),
    '#default_value' => content_access_get_settings('view_own', $type),
  );
  $form['type']['update_own'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Edit own content'),
    '#default_value' => content_access_get_settings('update_own', $type),
  );
  $form['type']['delete_own'] = array('#type' => 'checkboxes',
    '#prefix' => '<div class="content_access-div">',
    '#suffix' => '</div>',
    '#options' => $roles,
    '#title' => t('Delete own content'),
    '#default_value' => content_access_get_settings('delete_own', $type),
  );
  $form['type']['clearer'] = array(
    '#value' => '<br clear="all" />',
  );
  // Per node:
  $form['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Per content access control settings'),
    '#collapsible' => TRUE,
    '#description' => t('Optionally you can enable per content access control settings. If enabled, a new tab for the content access settings appears when viewing content. You have to configure permission to access these settings at the !permissions page.', array('!permissions' => l(t('permissions'), 'admin/user/permissions'))),
  );
  $form['node']['per_node'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable per content node access control settings'),
    '#default_value' => content_access_get_settings('per_node', $type),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced']['priority'] = array(
    '#type' => 'weight',
    '#title' => t('Give content node grants priority'),
    '#default_value' => content_access_get_settings('priority', $type),
    '#description' => t('If you are only using this access control module, you can safely ignore this. If you are using multiple access control modules you can adjust the priority of this module.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );
  return $form;
}

function content_access_admin_settings_submit($form_id, &$form_state) {
  // Where possible let the drupal permissions system handle access control.
  $permissions = content_access_get_permissions_by_role();
  foreach (array('update', 'update_own', 'delete', 'delete_own') as $op) {
    foreach ($form_state['values'][$op] as $rid => $value) {
      $permissions[$rid][ content_access_get_permission_by_op($op, $form_state['type']) ] = $value;
    }
    // Don't save the setting, so its default value (get permission) is applied always.
    unset($form_state['values'][$op]);
  }
  content_access_save_permissions($permissions);

  // Update content access settings
  $settings = content_access_get_settings();
  foreach (content_access_available_settings() as $setting) {
    unset($settings[$setting][$form_state['type']]);
    if (isset($form_state['values'][$setting])) {
      $settings[$setting][$form_state['type']] = is_array($form_state['values'][$setting]) ? array_filter($form_state['values'][$setting]) : $form_state['values'][$setting];
    }
  }
  content_access_set_settings($settings);

  // Mass update the nodes
  content_access_mass_update($form_state['type']);
  cache_clear_all();

  drupal_set_message('Your changes have been saved.');
}

/**
 * Mass updates node access records for nodes of the given type.
 */
function content_access_mass_update($type) {
  $result = db_query("SELECT n.nid FROM {node} n WHERE type = '%s'", $type);
  while ($node = db_fetch_object($result)) {
    node_access_acquire_grants(node_load($node->nid));
  }
}

/**
 * Gets the permissions for the role of the given id.
 */
function content_access_get_permissions_by_role() {
  $result = db_query('SELECT r.rid, p.perm FROM {role} r LEFT JOIN {permission} p ON r.rid = p.rid');
  $permissions = array();
  while ($role = db_fetch_object($result)) {
    $permissions[$role->rid] = array_filter(drupal_map_assoc(explode(', ', $role->perm)));
  }
  return $permissions;
}

/**
 * Saves the given permissions by role to the database.
 */
function content_access_save_permissions($permissions) {
  foreach ($permissions as $rid => $perms) {
    $perms = array_filter($perms);
    db_query('DELETE FROM {permission} WHERE rid = %d', $rid);
    if (count($perms)) {
      db_query("INSERT INTO {permission} (rid, perm) VALUES (%d, '%s')", $rid, implode(', ', array_keys($perms)));
    }
  }
}

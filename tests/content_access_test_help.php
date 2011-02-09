<?php
// $Id$

/**
 * @file
 * Helper class with auxiliary functions for content access module tests
 */

class ContentAccessTestCase extends DrupalWebTestCase {

  var $test_user;
  var $admin_user;
  var $content_type;
  var $url_content_type_name;
  var $node1;
  var $node2;

  /**
   * Preparation work that is done before each test.
   * Test users, content types, nodes etc. are created.
   */
  function setUp($module = '') {
    if (empty($module)) {
      // Enable content access module
      parent::setUp('content_access');
    }
    else {
      // Enable content access module plus another module
      parent::setUp('content_access', $module);
      // Stop setup when module could not be enabled
      if (!module_exists($module)) {
        return;
      }
    }

    // Create test user with seperate role
    $this->test_user = $this->drupalCreateUser();

    // Create admin user
    $this->admin_user = $this->drupalCreateUser(array('access content', 'administer content types', 'grant content access', 'grant own content access', 'administer nodes', 'access administration pages'));
    $this->drupalLogin($this->admin_user);

    // Rebuild content access permissions
    node_access_rebuild();

    // Create test content type
    $this->content_type = $this->drupalCreateContentType();
  }

  /**
   * Change access permissions for a content type
   */
  function changeAccessContentType($access_settings) {
    $this->drupalPost('admin/structure/types/manage/'. $this->content_type->type .'/access', $access_settings, t('Submit'));
    $this->assertText(t('Your changes have been saved.'), 'access rules of content type were updated successfully');
  }

  /**
   * Change access permissions for a content type by a given keyword (view, update or delete)
   * for the role of the user
   */
  function changeAccessContentTypeKeyword($keyword, $access = TRUE, $user = NULL) {
    if ($user === NULL) {
      $user = $this->test_user;
    }
    $roles = $user->roles;
    // Make sure to get a role from the end, thus not the authenticated users.
    end($roles);
    $access_settings = array(
      $keyword .'['. key($roles) .']' => $access,
    );
    $this->changeAccessContentType($access_settings);
  }

  /**
   * Change the per node access setting for a content type
   */
  function changeAccessPerNode($access = TRUE) {
    $access_permissions = array(
      'per_node' => $access,
    );
    $this->changeAccessContentType($access_permissions);
  }

  /**
   * Change access permissions for a node by a given keyword (view, update or delete)
   */
  function changeAccessNodeKeyword($node, $keyword, $access = TRUE) {
    $user = $this->test_user;
    $roles = $user->roles;
    // Make sure to get a role from the end, thus not the authenticated users.
    end($roles);
    $access_settings = array(
      $keyword .'['. key($roles) .']' => $access,
    );
    $this->changeAccessNode($node, $access_settings);
  }

  /**
   * Change access permission for a node
   */
  function changeAccessNode($node, $access_settings) {
    $this->drupalPost('node/'. $node->nid .'/access', $access_settings, t('Submit'));
    $this->assertText(t('Your changes have been saved.'), 'access rules of node were updated successfully');
  }
}
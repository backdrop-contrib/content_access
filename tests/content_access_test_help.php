<?php

/**
 * @file
 * Helper class with auxiliary functions for content access module tests
 */

class ContentAccessTestCase extends BackdropWebTestCase {

  var $test_user;
  var $rid;
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
        $this->pass('No ' . $module . ' module present, skipping test');
        return;
      }
    }

    // Create test user with seperate role
    $this->test_user = $this->backdropCreateUser(array('access content'));

    // Get the value of the new role
    // Needed in D7 because it's by default create two roles for new users
    // one role is Authenticated and the second is new default one
    // @see backdropCreateUser()
    foreach ($this->test_user->roles as $role) {
      if (!in_array($role, array(BACKDROP_AUTHENTICATED_ROLE))) {
        $this->rid = $role;
        break;
      }
    }

    // Create admin user
    $this->admin_user = $this->backdropCreateUser(array('access content', 'administer content types', 'grant content access', 'grant own content access', 'administer nodes', 'access administration pages'));
    $this->backdropLogin($this->admin_user);

    // Rebuild content access permissions
    node_access_rebuild();

    // Create test content type
    $this->content_type = $this->backdropCreateContentType();
  }

  /**
   * Change access permissions for a content type
   */
  function changeAccessContentType($access_settings) {
    $this->backdropPost('admin/structure/types/manage/'. $this->content_type->type .'/access', $access_settings, t('Submit'));
    $this->assertText(t('Your changes have been saved.'), 'access rules of content type were updated successfully');
  }

  /**
   * Change access permissions for a content type by a given keyword (view, update or delete)
   * for the role of the user
   */
  function changeAccessContentTypeKeyword($keyword, $access = TRUE, $user = NULL) {
    if ($user === NULL) {
      $user = $this->test_user;
      $roles[$this->rid] = $this->rid;
    } else {
      foreach ($user->roles as $role) {
        if (!in_array($role, array(BACKDROP_AUTHENTICATED_ROLE))) {
          $roles[$role] = $role;
          break;
        }
      }
    }

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
    $roles[$this->rid] = $this->rid;

    $access_settings = array(
      $keyword .'['. key($roles) .']' => $access,
    );

    $this->changeAccessNode($node, $access_settings);
  }

  /**
   * Change access permission for a node
   */
  function changeAccessNode($node, $access_settings) {
    $this->backdropPost('node/'. $node->nid .'/access', $access_settings, t('Save configuration'));
    $this->assertText(t('Your changes have been saved.'), 'access rules of node were updated successfully');
  }
}
<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _chargify_sites_content_default_fields() {
  $fields = array();

  // Exported field: field_domain
  $fields[] = array(
    'field_name' => 'field_domain',
    'type_name' => 'site',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'content_bottom' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'content_top' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'right' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'link',
    'required' => '1',
    'multiple' => '0',
    'module' => 'link',
    'active' => '1',
    'attributes' => array(
      'target' => 'default',
      'rel' => '',
      'class' => '',
    ),
    'display' => array(
      'url_cutoff' => '80',
    ),
    'url' => 0,
    'title' => 'none',
    'title_value' => '',
    'enable_tokens' => 0,
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'title' => '',
          'url' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Domain name',
      'weight' => '31',
      'description' => '',
      'type' => 'link',
      'module' => 'link',
    ),
  );

  // Translatables
  array(
    t('Domain name'),
  );

  return $fields;
}

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _chargify_sites_user_default_permissions() {
  $permissions = array();

  // Exported permission: administer chargify sites
  $permissions[] = array(
    'name' => 'administer chargify sites',
    'roles' => array(
      '0' => 'Admin',
    ),
  );

  // Exported permission: create site content
  $permissions[] = array(
    'name' => 'create site content',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: delete any site content
  $permissions[] = array(
    'name' => 'delete any site content',
    'roles' => array(
      '0' => 'Admin',
    ),
  );

  // Exported permission: delete own site content
  $permissions[] = array(
    'name' => 'delete own site content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit any site content
  $permissions[] = array(
    'name' => 'edit any site content',
    'roles' => array(
      '0' => 'Admin',
    ),
  );

  // Exported permission: edit own site content
  $permissions[] = array(
    'name' => 'edit own site content',
    'roles' => array(),
  );

  return $permissions;
}

<?php

/**
 * @file
 * Default Rules configuration.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_dunning_defaults_default_rules_configuration() {
  $items = array();

  // Component: Rule: Send dunning notification.
  $items['rules_commerce_dunning_send_notification'] = entity_import('rules_config', '{ "rules_commerce_dunning_send_notification" : {
      "LABEL" : "Send dunning notification",
      "PLUGIN" : "rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "rules", "message_notify" ],
      "USES VARIABLES" : {
        "message" : { "label" : "Message", "type" : "message" },
        "order" : { "label" : "Order", "type" : "commerce_order" },
        "card" : { "label" : "Card", "type" : "commerce_cardonfile" }
      },
      "IF" : [
        { "entity_has_field" : { "entity" : [ "message" ], "field" : "commerce_dunning_message_order" } },
        { "entity_has_field" : { "entity" : [ "message" ], "field" : "commerce_dunning_message_card" } }
      ],
      "DO" : [
        { "data_set" : {
            "data" : [ "message:commerce_dunning_message_order" ],
            "value" : [ "order" ]
          }
        },
        { "data_set" : {
            "data" : [ "message:commerce-dunning-message-card" ],
            "value" : [ "card" ]
          }
        },
        { "entity_save" : { "data" : [ "message" ], "immediate" : 1 } },
        { "message_notify_process" : { "message" : [ "message" ] } }
      ]
    }
  }');

  // Component: Rule: Send dunning notification — no card available.
  $items['rules_commerce_dunning_send_notification_no_card'] = entity_import('rules_config', '{ "rules_commerce_dunning_send_notification_no_card" : {
      "LABEL" : "Send dunning notification \u2014 no card available",
      "PLUGIN" : "rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "rules", "message_notify" ],
      "USES VARIABLES" : {
        "message" : { "label" : "Message", "type" : "message" },
        "order" : { "label" : "Order", "type" : "commerce_order" }
      },
      "IF" : [
        { "entity_has_field" : { "entity" : [ "message" ], "field" : "commerce_dunning_message_order" } }
      ],
      "DO" : [
        { "data_set" : {
            "data" : [ "message:commerce_dunning_message_order" ],
            "value" : [ "order" ]
          }
        },
        { "entity_save" : { "data" : [ "message" ], "immediate" : 1 } },
        { "message_notify_process" : { "message" : [ "message" ] } }
      ]
    }
  }');

  // Reaction rule: Notify customer about a hard declined transaction.
  $items['rules_commerce_dunning_send_notification_hard_declined'] = entity_import('rules_config', '{ "rules_commerce_dunning_send_notification_hard_declined" : {
      "LABEL" : "Notify customer about a hard declined transaction",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "rules", "commerce_cardonfile" ],
      "ON" : [ "commerce_cardonfile_charge_failed" ],
      "IF" : [
        { "data_is" : {
            "data" : [ "response:code" ],
            "op" : "IN",
            "value" : { "value" : {
                "card_not_chargeable" : "card_not_chargeable",
                "no_method" : "no_method",
                "method_not_capable" : "method_not_capable",
                "card_expired" : "card_expired",
                "method_failure_hard_decline" : "method_failure_hard_decline"
              }
            }
          }
        }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_dunning_hard_declined_card",
              "param_user" : [ "order:owner" ]
            },
            "PROVIDE" : { "entity_created" : { "message_entity_created" : "Created message entity" } }
          }
        },
        { "component_rules_commerce_dunning_send_notification" : {
            "message" : [ "message-entity-created" ],
            "order" : [ "order" ],
            "card" : [ "response:card-chosen" ]
          }
        },
        { "data_set" : { "data" : [ "response:card-chosen:status" ], "value" : "3" } }
      ]
    }
  }');

  // Reaction rule: Notify customer about a soft declined transaction.
  $items['rules_commerce_dunning_send_notification_soft_declined'] = entity_import('rules_config', '{ "rules_commerce_dunning_send_notification_soft_declined" : {
      "LABEL" : "Notify customer about a soft declined transaction",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "rules", "commerce_cardonfile" ],
      "ON" : [ "commerce_cardonfile_charge_failed" ],
      "IF" : [
        { "data_is" : {
            "data" : [ "response:code" ],
            "op" : "IN",
            "value" : { "value" : {
                "method_failure" : "method_failure",
                "method_failure_insufficient_funds" : "method_failure_insufficient_funds",
                "method_failure_limit_exceeded" : "method_failure_limit_exceeded",
                "method_failure_call_issuer" : "method_failure_call_issuer",
                "method_failure_temporary_hold" : "method_failure_temporary_hold",
                "method_failure_generic_decline" : "method_failure_generic_decline",
                "method_failure_gateway_error" : "method_failure_gateway_error",
                "method_failure_gateway_unavailable" : "method_failure_gateway_unavailable",
                "method_failure_gateway_configuration_error" : "method_failure_gateway_configuration_error"
              }
            }
          }
        }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_dunning_soft_declined_card",
              "param_user" : [ "order:owner" ]
            },
            "PROVIDE" : { "entity_created" : { "message_entity_created" : "Created message entity" } }
          }
        },
        { "component_rules_commerce_dunning_send_notification" : {
            "message" : [ "message-entity-created" ],
            "order" : [ "order" ],
            "card" : [ "response:card-chosen" ]
          }
        }
      ]
    }
  }');

  // Reaction rule: Notify customer about a non-available card.
  $items['rules_commerce_dunning_send_notification_no_card_available'] = entity_import('rules_config', '{ "rules_commerce_dunning_send_notification_no_card_available" : {
      "LABEL" : "Notify customer about a non-available card",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "rules", "commerce_cardonfile" ],
      "ON" : [ "commerce_cardonfile_charge_failed" ],
      "IF" : [
        { "data_is" : {
            "data" : [ "response:code" ],
            "op" : "IN",
            "value" : { "value" : { "insufficient" : "insufficient", "card_na" : "card_na" } }
          }
        }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_dunning_no_card",
              "param_user" : [ "order:owner" ]
            },
            "PROVIDE" : { "entity_created" : { "message_entity_created" : "Created message entity" } }
          }
        },
        { "component_rules_commerce_dunning_send_notification_no_card" : { "message" : [ "message-entity-created" ], "order" : [ "order" ] } }
      ]
    }
  }');

  return $items;
}

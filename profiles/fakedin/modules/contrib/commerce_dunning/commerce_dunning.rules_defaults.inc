<?php

/**
 * @file
 * Default Rules configuration.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_dunning_default_rules_configuration() {
  $items = array();

  // Reaction rule: Schedule charge attempts.
  $items['rules_commerce_dunning_schedule_charge_attempts'] = entity_import('rules_config', '{ "rules_commerce_dunning_schedule_charge_attempts" : {
      "LABEL" : "Schedule charge attempts",
      "PLUGIN" : "reaction rule",
      "WEIGHT" : "-10",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "commerce_dunning", "rules", "rules_scheduler", "commerce_cardonfile" ],
      "ON" : [ "commerce_cardonfile_charge_failed" ],
      "IF" : [
        { "NOT commerce_dunning_order_revision_status" : {
            "order" : [ "order" ],
            "status" : { "value" : {
                "cardonfile_payment_error_hard_decline" : "cardonfile_payment_error_hard_decline",
                "cardonfile_payment_failed_soft_decline" : "cardonfile_payment_failed_soft_decline"
              }
            }
          }
        }
      ],
      "DO" : [
        { "schedule" : {
            "component" : "rules_commerce_dunning_charge_card",
            "date" : "+3 days",
            "identifier" : "dunning-[order:order-id]-charge-attempt-3-days",
            "param_order" : [ "order" ]
          }
        },
        { "schedule" : {
            "component" : "rules_commerce_dunning_charge_card",
            "date" : "+10 days",
            "identifier" : "dunning-[order:order-id]-charge-attempt-10-days",
            "param_order" : [ "order" ]
          }
        },
        { "schedule" : {
            "component" : "rules_commerce_dunning_charge_card",
            "date" : "+14 days",
            "identifier" : "dunning-[order:order-id]-charge-attempt-14-days",
            "param_order" : [ "order" ]
          }
        },
        { "schedule" : {
            "component" : "rules_commerce_dunning_terminate_dunning_cycle",
            "date" : "+15 days",
            "identifier" : "dunning-[order:order-id]-end-15-days",
            "param_order" : [ "order" ]
          }
        }
      ]
    }
  }');

  // Component: Rule: Charge card.
  $items['rules_commerce_dunning_charge_card'] = entity_import('rules_config', '{ "rules_commerce_dunning_charge_card" : {
      "LABEL" : "Charge card",
      "PLUGIN" : "rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "commerce_payment", "commerce_cardonfile" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "IF" : [
        { "commerce_payment_order_balance_comparison" : { "commerce_order" : [ "order" ], "operator" : "\u003E", "value" : "0" } }
      ],
      "DO" : [
        { "commerce_cardonfile_order_select_card" : {
            "USING" : { "order" : [ "order" ] },
            "PROVIDE" : { "select_card_response" : { "select_card_response" : "Select card response" } }
          }
        },
        { "commerce_cardonfile_order_charge_card" : {
            "USING" : {
              "order" : [ "order" ],
              "charge" : [ "" ],
              "select_card_response" : [ "select-card-response" ],
              "card_data" : [ "" ]
            },
            "PROVIDE" : { "charge_card_response" : { "charge_card_response" : "Charge Card Response" } }
          }
        }
      ]
    }
  }');

  // Reaction rule: Unschedule all further dunning steps.
  $items['rules_commerce_dunning_unschedule_all_further_steps'] = entity_import('rules_config', '{ "rules_commerce_dunning_unschedule_all_further_steps" : {
      "LABEL" : "Unschedule all further dunning steps",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "commerce_payment", "commerce_dunning", "commerce_cardonfile" ],
      "ON" : [ "commerce_cardonfile_charge_success" ],
      "IF" : [
        { "commerce_payment_order_balance_comparison" : { "commerce_order" : [ "order" ], "value" : "0" } }
      ],
      "DO" : [ { "commerce_dunning_unschedule_tasks" : { "order" : [ "order" ] } } ]
    }
  }');

  // Component: Action set: Terminate dunning cycle.
  $items['rules_commerce_dunning_terminate_dunning_cycle'] = entity_import('rules_config', '{ "rules_commerce_dunning_terminate_dunning_cycle" : {
      "LABEL" : "Terminate dunning cycle",
      "PLUGIN" : "action set",
      "TAGS" : [ "Commerce Dunning" ],
      "REQUIRES" : [ "commerce_dunning", "rules" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_dunning_invoke_dunning_end" : { "order" : [ "order" ] } },
        { "data_set" : { "data" : [ "order:status" ], "value" : "dunning_past_due" } },
        { "commerce_dunning_unschedule_tasks" : { "order" : [ "order" ] } }
      ]
    }
  }');

  return $items;
}

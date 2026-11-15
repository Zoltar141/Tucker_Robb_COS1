**Purpose:** Standard starting structure for creating JSON configuration files within Wonkle.

This is a fill-in-the-blank template. Replace placeholders with actual values, remove optional sections that aren't needed, and preserve the overall organization where applicable.
## 1. Complete Configuration Template

{
  "metadata": {
    "name": "FILL_IN_NAME",
    "version": "1.0.0",
    "description": "FILL_IN_DESCRIPTION",
    "type": "FILL_IN_TYPE"
  },

  "game": {
    "name": "FILL_IN_GAME_NAME",

    "settings": {
      "width": 0,
      "height": 0,
      "difficulty": "FILL_IN_DIFFICULTY",
      "starting_lives": 0,
      "starting_score": 0
    },

    "controls": {
      "up": "FILL_IN_KEY",
      "down": "FILL_IN_KEY",
      "left": "FILL_IN_KEY",
      "right": "FILL_IN_KEY",
      "action": "FILL_IN_KEY"
    },

    "state": {
      "FILL_IN_STATE_NAME": 0,
      "FILL_IN_STATE_NAME_2": 0
    },

    "rules": [
      {
        "name": "FILL_IN_RULE_NAME",
        "condition": {
          "variable": "FILL_IN_STATE_VARIABLE",
          "operator": "FILL_IN_OPERATOR",
          "value": 0
        },
        "transformations": [
          {
            "variable": "FILL_IN_TARGET_VARIABLE",
            "operation": "FILL_IN_OPERATION",
            "value": 0
          }
        ]
      }
    ],

    "rendering": {
      "dimensions": {
        "width": 0,
        "height": 0
      },

      "world_vertical_offset": 0,

      "roles": {
        "background": "FILL_IN_THEME_ROLE",
        "foreground": "FILL_IN_THEME_ROLE",
        "boundary": "FILL_IN_THEME_ROLE",
        "player": "FILL_IN_THEME_ROLE",
        "object": "FILL_IN_THEME_ROLE"
      },

      "characters": {
        "player": 0,
        "object": 0,
        "boundary": 0
      }
    },

    "boundaries": {
      "enabled": true,
      "left": 0,
      "right": 0,
      "top": 0,
      "bottom": 0
    },

    "collision": {
      "enabled": true,
      "rules": []
    }
  }
}
## 2.Placeholder Reference

|Placeholder|Description|
|---|---|
|`FILL_IN_NAME`|Configuration identifier|
|`FILL_IN_TYPE`|Configuration category|
|`FILL_IN_GAME_NAME`|Name of the game or system|
|`FILL_IN_DIFFICULTY`|Starting difficulty|
|`FILL_IN_KEY`|Input binding|
|`FILL_IN_STATE_NAME`|State variable name|
|`FILL_IN_RULE_NAME`|Unique rule identifier|
|`FILL_IN_STATE_VARIABLE`|Variable evaluated by a condition|
|`FILL_IN_OPERATOR`|Comparison operator|
|`FILL_IN_TARGET_VARIABLE`|Variable modified by a transformation|
|`FILL_IN_OPERATION`|Transformation operation|
|`FILL_IN_THEME_ROLE`|Semantic theme role|
## 3.Configuration Guidelines

1. Use consistent naming conventions.
    
2. Keep game-specific settings inside the game configuration.
    
3. Use state variables to represent runtime data.
    
4. Use rules and transformations for behavior supported by `WonkleRuntime`.
    
5. Reference semantic theme roles rather than hardcoding colors.
    
6. Use numeric character codes where the renderer expects character codes.
    
7. Keep collision and boundary configuration separate.
    
8. Remove unused optional sections rather than filling them with meaningless values.
    
9. Ensure every JSON file is valid before integrating it into Wonkle.
    
10. Do not introduce new configuration properties without defining how the framework will consume them.

**Important:** This blueprint is a starting convention, not a requirement that every configuration contain every section. Only properties supported by the consuming Wonkle system should be included.
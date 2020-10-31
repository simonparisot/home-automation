exports.handler = function (request, context) {

    // two types of request are possible : POWERCONTROL or DISCOVERY
    if (request.directive.header.namespace === 'Alexa.PowerController') {
        if (request.directive.header.name === 'TurnOn' || request.directive.header.name === 'TurnOff') {
            log("DEBUG:", "Power control request", JSON.stringify(request));
            handlePowerControl(request, context);
        }
    }
    else if (request.directive.header.namespace === 'Alexa.Discovery' && request.directive.header.name === 'Discover') {
        log("DEBUG:", "Discover request",  JSON.stringify(request));
        handleDiscovery(request, context, "");
    }

    // POWER CONTROL a device = connect to MQTT and send a message on the command topic
    function handlePowerControl(request, context) {

        // connect to MQTT
        log("CONNECTING TO MQTT...");
        var mqtt = require('mqtt');
        var MQTT_ADDR = "mqtt://{{CLOUDMQTT_USER}}:{{CLOUDMQTT_PASSWORD}}@m23.cloudmqtt.com:15067";
        var client = mqtt.connect(MQTT_ADDR,{clientId: 'awslambda', protocolId: 'MQIsdp', protocolVersion: 3, connectTimeout:1000, debug:true});

        // when connected
        client.on('connect', function () {
            
            // what light should we switch ?
            var light = request.directive.endpoint.endpointId;
            var reqAction = request.directive.header.name;

            // prepare command
            log("CONNECTED, SENDING COMMAND MESSAGE...")
            if      (light == "light-001") { var commandMsg = "1"; }
            else if (light == "light-004") { var commandMsg = "CUI;"; commandMsg += (reqAction == "TurnOff")?"OF":"ON"; }
            else if (light == "light-005") { var commandMsg = "PDT;"; commandMsg += (reqAction == "TurnOff")?"OF":"ON"; }
            else if (light == "light-003") { var commandMsg = "TAB;"; commandMsg += (reqAction == "TurnOff")?"OF":"ON"; }
            else if (light == "light-002") { var commandMsg = "SAL;"; commandMsg += (reqAction == "TurnOff")?"OF":"ON"; }

            // send command
            log(commandMsg);
            client.publish('command', commandMsg);
            client.end();

            // prepare response
            var responseHeader = request.directive.header;
            responseHeader.namespace = "Alexa";
            responseHeader.name = "Response";
            responseHeader.messageId = responseHeader.messageId + "-R";
            
            // get user token pass in request (TO DO: USE IT TO COMMUNICATE WITH THE DEVICE)
            var requestToken = request.directive.endpoint.scope.token;
            
            /* TO UPDATE
            var powerResult;
            if (requestMethod === "TurnOn") {
                powerResult = "ON";
            }
            else if (requestMethod === "TurnOff") {
                powerResult = "OFF";
            } */

            var contextResult = {
                "properties": [{
                    "namespace": "Alexa.PowerController",
                    "name": "powerState",
                    "value": "ON",
                    "timeOfSample": "2017-09-03T16:20:50.52Z", //retrieve from result.
                    "uncertaintyInMilliseconds": 50
                }]
            };

            var response = {
                context: contextResult,
                event: {
                    header: responseHeader,
                    endpoint: {
                        scope: {
                            type: "BearerToken",
                            token: requestToken
                        },
                        endpointId: "light-001"
                    },
                    payload: {}
                }
            };
            log("DEBUG", "Alexa.PowerController ", JSON.stringify(response));
            context.succeed(response);
        });
    }

    // DISCOVER house devices = simply send a json description
    function handleDiscovery(request, context) {
        var payload = {
            "endpoints":
            [
                {
                    "endpointId": "light-001",
                    "manufacturerName": "Simon",
                    "friendlyName": "de la chambre",
                    "description": "Connected light by Simon",
                    "displayCategories": ["LIGHT"],
                    "cookie": {
                        "key1": "arbitrary key/value pairs for skill to reference this endpoint.",
                        "key2": "There can be multiple entries",
                        "key3": "but they should only be used for reference purposes.",
                        "key4": "This is not a suitable place to maintain current endpoint state."
                    },
                    "capabilities":
                    [
                        {
                          "type": "AlexaInterface",
                          "interface": "Alexa",
                          "version": "3"
                        },
                        {
                            "interface": "Alexa.PowerController",
                            "version": "3",
                            "type": "AlexaInterface",
                            "properties": {
                                "supported": [{
                                    "name": "powerState"
                                }],
                                 "retrievable": true
                            }
                        }
                    ]
                },
                {
                    "endpointId": "light-002",
                    "manufacturerName": "Simon",
                    "friendlyName": "du salon",
                    "description": "Connected light by Simon",
                    "displayCategories": ["LIGHT"],
                    "cookie": {
                        "key1": "arbitrary key/value pairs for skill to reference this endpoint.",
                        "key2": "There can be multiple entries",
                        "key3": "but they should only be used for reference purposes.",
                        "key4": "This is not a suitable place to maintain current endpoint state."
                    },
                    "capabilities":
                    [
                        {
                          "type": "AlexaInterface",
                          "interface": "Alexa",
                          "version": "3"
                        },
                        {
                            "interface": "Alexa.PowerController",
                            "version": "3",
                            "type": "AlexaInterface",
                            "properties": {
                                "supported": [{
                                    "name": "powerState"
                                }],
                                 "retrievable": true
                            }
                        }
                    ]
                },
                {
                    "endpointId": "light-003",
                    "manufacturerName": "Simon",
                    "friendlyName": "de la table",
                    "description": "Connected light by Simon",
                    "displayCategories": ["LIGHT"],
                    "cookie": {
                        "key1": "arbitrary key/value pairs for skill to reference this endpoint.",
                        "key2": "There can be multiple entries",
                        "key3": "but they should only be used for reference purposes.",
                        "key4": "This is not a suitable place to maintain current endpoint state."
                    },
                    "capabilities":
                    [
                        {
                          "type": "AlexaInterface",
                          "interface": "Alexa",
                          "version": "3"
                        },
                        {
                            "interface": "Alexa.PowerController",
                            "version": "3",
                            "type": "AlexaInterface",
                            "properties": {
                                "supported": [{
                                    "name": "powerState"
                                }],
                                 "retrievable": true
                            }
                        }
                    ]
                },
                {
                    "endpointId": "light-004",
                    "manufacturerName": "Simon",
                    "friendlyName": "de la cuisine",
                    "description": "Connected light by Simon",
                    "displayCategories": ["LIGHT"],
                    "cookie": {
                        "key1": "arbitrary key/value pairs for skill to reference this endpoint.",
                        "key2": "There can be multiple entries",
                        "key3": "but they should only be used for reference purposes.",
                        "key4": "This is not a suitable place to maintain current endpoint state."
                    },
                    "capabilities":
                    [
                        {
                          "type": "AlexaInterface",
                          "interface": "Alexa",
                          "version": "3"
                        },
                        {
                            "interface": "Alexa.PowerController",
                            "version": "3",
                            "type": "AlexaInterface",
                            "properties": {
                                "supported": [{
                                    "name": "powerState"
                                }],
                                 "retrievable": true
                            }
                        }
                    ]
                },
                {
                    "endpointId": "light-005",
                    "manufacturerName": "Simon",
                    "friendlyName": "du plan de travail",
                    "description": "Connected light by Simon",
                    "displayCategories": ["LIGHT"],
                    "cookie": {
                        "key1": "arbitrary key/value pairs for skill to reference this endpoint.",
                        "key2": "There can be multiple entries",
                        "key3": "but they should only be used for reference purposes.",
                        "key4": "This is not a suitable place to maintain current endpoint state."
                    },
                    "capabilities":
                    [
                        {
                          "type": "AlexaInterface",
                          "interface": "Alexa",
                          "version": "3"
                        },
                        {
                            "interface": "Alexa.PowerController",
                            "version": "3",
                            "type": "AlexaInterface",
                            "properties": {
                                "supported": [{
                                    "name": "powerState"
                                }],
                                 "retrievable": true
                            }
                        }
                    ]
                }
            ]
        };
        var header = request.directive.header;
        header.name = "Discover.Response";
        log("DEBUG", "Discovery Response: ", JSON.stringify({ header: header, payload: payload }));
        context.succeed({ event: { header: header, payload: payload } });
    }

    // Simple log function
    function log(message, message1, message2) {
        console.log(message + message1 + message2);
    }
};
var AWS = require('aws-sdk');
var iotdata = new AWS.IotData({ endpoint: 'a377qra5nhpqt6-ats.iot.eu-west-1.amazonaws.com' });

exports.handler = async(event, context) => {
    
    if (event.directive.header.namespace === 'Alexa.Discovery' && event.directive.header.name === 'Discover') {
        log("DEBUG:", "Discover request",  JSON.stringify(event));
        handleDiscovery(event, context, "");
    }
    else if (event.directive.header.namespace === 'Alexa.PowerController') {
        if (event.directive.header.name === 'TurnOn' || event.directive.header.name === 'TurnOff') {
            log("DEBUG:", "TurnOn or TurnOff Request", JSON.stringify(event));
            await handlePowerControl(event, context);
        }
    }

    async function handlePowerControl(event, context) {
        // get device ID passed in during discovery
        var requestMethod = event.directive.header.name;
        var responseHeader = event.directive.header;
        responseHeader.namespace = "Alexa";
        responseHeader.name = "Response";
        responseHeader.messageId = responseHeader.messageId + "-R";
        // get user token pass in event
        var requestToken = event.directive.endpoint.scope.token;
        var powerResult;

        if (requestMethod === "TurnOn") {

            let payload = '{"thing_type":"screen","thing_serial":"0","command":"unroll"}';
            let params = {
                topic: "command",
                payload: payload,
            };
            await iotdata.publish(params).promise();
            powerResult = "ON";
        }
        else if (requestMethod === "TurnOff") {

            let payload = '{"thing_type":"screen","thing_serial":"0","command":"roll"}';
            let params = {
                topic: "command",
                payload: payload,
            };
            await iotdata.publish(params).promise();
            powerResult = "OFF";
        }

        var contextResult = {
            "properties": [{
                "namespace": "Alexa.PowerController",
                "name": "powerState",
                "value": powerResult,
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
                    endpointId: "home-cinema"
                },
                payload: {}
            }
        };
        log("DEBUG", "Alexa.PowerController ", JSON.stringify(response));
        context.succeed(response);
    }

    function handleDiscovery(event, context) {
        var payload = {
            "endpoints":
            [
                {
                    "endpointId": "home-cinema",
                    "manufacturerName": "Simon",
                    "friendlyName": "Home cinema",
                    "description": "Connected home cinema by Simon",
                    "displayCategories": ["SWITCH"],
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
        var header = event.directive.header;
        header.name = "Discover.Response";
        log("DEBUG", "Discovery Response: ", JSON.stringify({ header: header, payload: payload }));
        context.succeed({ event: { header: header, payload: payload } });
    }

    function log(message, message1, message2) {
        console.log(message + message1 + message2);
    }
};
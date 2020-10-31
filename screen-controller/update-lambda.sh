cd alexa-lambda-function
zip -r code *
mv code.zip ..
cd ..
aws lambda update-function-code --function-name "alexa-home-cinema-skill" --zip-file "fileb://code.zip" --region eu-west-1 --profile=perso
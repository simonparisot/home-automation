cd AlexaBank
zip -r code *
mv code.zip ..
cd ..
aws lambda update-function-code --function-name "AlexaBank" --zip-file "fileb://code.zip" --region eu-west-1 --profile=perso

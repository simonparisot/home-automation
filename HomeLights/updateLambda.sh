cd myHomeLights
zip -r code *
mv code.zip ..
cd ..
aws lambda update-function-code --function-name "myHomeLights" --zip-file "fileb://code.zip" --region eu-west-1 --profile=perso

#!/bin/bash
set -e

REGION="us-east-1"
AMI_ID="ami-08c40ec9ead489470" # Ubuntu 22.04, adjust if needed
INSTANCE_TYPE="c6i.large"
KEY_NAME="orderbook-key"
SG_NAME="orderbook-sg"
USER_DATA_FILE="scripts/cloud_init.sh"

# 1️⃣ Ensure default VPC exists
VPC_ID=$(aws ec2 describe-vpcs \
  --region $REGION \
  --query "Vpcs[?IsDefault].VpcId | [0]" \
  --output text)

if [ "$VPC_ID" == "None" ] || [ -z "$VPC_ID" ]; then
    echo "No default VPC found. Run aws_bootstrap.sh first."
    exit 1
fi
echo "Using VPC: $VPC_ID"

# 2️⃣ Ensure Security Group exists in this VPC
SG_ID=$(aws ec2 describe-security-groups \
  --filters "Name=group-name,Values=$SG_NAME" "Name=vpc-id,Values=$VPC_ID" \
  --query "SecurityGroups[0].GroupId" --output text)

if [ "$SG_ID" == "None" ] || [ -z "$SG_ID" ]; then
    echo "Security group not found. Creating..."
    SG_ID=$(aws ec2 create-security-group \
      --group-name $SG_NAME \
      --description "OrderBook benchmark SG" \
      --vpc-id $VPC_ID \
      --region $REGION \
      --query "GroupId" --output text)

    # Allow SSH
    aws ec2 authorize-security-group-ingress \
      --group-id $SG_ID \
      --protocol tcp --port 22 --cidr 0.0.0.0/0 \
      --region $REGION
fi
echo "Using Security Group ID: $SG_ID"

# 3️⃣ Ensure key pair exists locally
if [ ! -f "$KEY_NAME.pem" ]; then
    echo "Key pair $KEY_NAME.pem not found locally. Run aws_bootstrap.sh first."
    exit 1
fi

# 4️⃣ Launch spot instance
echo "Requesting spot instance..."
INSTANCE_ID=$(aws ec2 run-instances \
  --image-id $AMI_ID \
  --instance-type $INSTANCE_TYPE \
  --key-name $KEY_NAME \
  --security-group-ids $SG_ID \
  --user-data file://scripts/cloud_init.sh \
  --region $REGION \
  --query 'Instances[0].InstanceId' \
  --output text)

echo "Launched instance: $INSTANCE_ID"

# 5️⃣ Wait for instance to be running
aws ec2 wait instance-running --instance-ids $INSTANCE_ID --region $REGION

# 6️⃣ Get public IP
PUBLIC_IP=$(aws ec2 describe-instances \
  --instance-ids $INSTANCE_ID \
  --region $REGION \
  --query 'Reservations[0].Instances[0].PublicIpAddress' \
  --output text)
echo "Instance is running at: $PUBLIC_IP"

# 7️⃣ Wait until instance auto-shuts down (after cloud-init benchmark)
echo "Waiting for instance to terminate after benchmark..."
aws ec2 wait instance-terminated --instance-ids $INSTANCE_ID --region $REGION

echo "Benchmark complete. Instance terminated."

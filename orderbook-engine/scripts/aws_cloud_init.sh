#!/bin/bash
set -e

REGION="us-east-1"
KEY_NAME="orderbook-key"
SG_NAME="orderbook-sg"

# 1️⃣ Check if default VPC exists, create if missing
DEFAULT_VPC_ID=$(aws ec2 describe-vpcs \
    --region $REGION \
    --query "Vpcs[?IsDefault].VpcId | [0]" \
    --output text)

if [ "$DEFAULT_VPC_ID" == "None" ] || [ -z "$DEFAULT_VPC_ID" ]; then
    echo "No default VPC found, creating one..."
    DEFAULT_VPC_ID=$(aws ec2 create-default-vpc --region $REGION --query "Vpc.VpcId" --output text)
    echo "Created default VPC: $DEFAULT_VPC_ID"
else
    echo "Default VPC exists: $DEFAULT_VPC_ID"
fi

# 2️⃣ Create Security Group in the default VPC (ignore error if exists)
aws ec2 create-security-group \
    --group-name $SG_NAME \
    --description "Security group for OrderBook engine" \
    --vpc-id $DEFAULT_VPC_ID \
    --region $REGION 2>/dev/null || echo "Security group $SG_NAME may already exist."

# 3️⃣ Allow SSH from anywhere
aws ec2 authorize-security-group-ingress \
    --group-name $SG_NAME \
    --protocol tcp \
    --port 22 \
    --cidr 0.0.0.0/0 \
    --region $REGION 2>/dev/null || echo "SSH rule may already exist."

# 4️⃣ Create EC2 key pair if missing
if [ ! -f "$KEY_NAME.pem" ]; then
    echo "Creating key pair $KEY_NAME..."
    aws ec2 create-key-pair \
        --key-name $KEY_NAME \
        --query 'KeyMaterial' \
        --output text \
        --region $REGION > "$KEY_NAME.pem"
    chmod 400 "$KEY_NAME.pem"
    echo "Key pair created and saved to $KEY_NAME.pem"
else
    echo "Key file $KEY_NAME.pem already exists locally."
fi

echo "✅ AWS bootstrap complete."
echo "You now have:"
echo "- Default VPC: $DEFAULT_VPC_ID"
echo "- Security Group: $SG_NAME"
echo "- Key Pair: $KEY_NAME.pem"

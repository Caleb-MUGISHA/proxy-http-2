module "vpc" {
  source = "../network/vpc"
  cidr_block = "10.0.0.0/16"
}

resource "aws_launch_template" "proxy" {
  name_prefix   = "proxy-template"
  image_id      = var.ami_id
  instance_type = "c5.large"
  key_name      = var.ssh_key_name

  network_interfaces {
    associate_public_ip_address = true
    security_groups             = [module.vpc.proxy_sg_id]
  }

  user_data = base64encode(templatefile("${path.module}/userdata.tpl", {
    git_repo   = var.proxy_repo
    config_s3  = aws_s3_bucket_object.config.id
  }))
}

resource "aws_autoscaling_group" "proxy" {
  desired_capacity    = 3
  max_size            = 10
  min_size            = 3
  vpc_zone_identifier = module.vpc.private_subnets

  launch_template {
    id      = aws_launch_template.proxy.id
    version = "$Latest"
  }

  tag {
    key                 = "Env"
    value               = "prod"
    propagate_at_launch = true
  }
}

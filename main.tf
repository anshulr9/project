terraform {
  required_version = ">= 1.3.0"
}

variable "env_name" {
  type = string
}

resource "null_resource" "env_check" {
  triggers = {
    workspace = terraform.workspace
    env_name  = var.env_name
  }
}

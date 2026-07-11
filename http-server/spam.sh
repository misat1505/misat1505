#!/bin/bash

# Default values
URL="http://localhost:8080/starship.mp4"
REQUESTS=1000
CONCURRENCY=100   # how many requests to send in parallel

usage() {
  echo "Usage: $0 [-u url] [-n requests] [-c concurrency]"
  echo "  -u   Target URL (default: $URL)"
  echo "  -n   Number of requests (default: $REQUESTS)"
  echo "  -c   Number of concurrent requests (default: $CONCURRENCY)"
  exit 1
}

while getopts "u:n:c:" opt; do
  case $opt in
    u) URL="$OPTARG" ;;
    n) REQUESTS="$OPTARG" ;;
    c) CONCURRENCY="$OPTARG" ;;
    *) usage ;;
  esac
done

echo "Spamming $URL with $REQUESTS requests ($CONCURRENCY concurrent)..."

# Run requests in parallel batches
seq "$REQUESTS" | xargs -n1 -P"$CONCURRENCY" -I{} bash -c "curl -s '$URL' > /dev/null || echo 'Request {} failed'"

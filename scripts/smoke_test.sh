#!/usr/bin/env bash

set -euo pipefail

API_BASE_URL="${1:-http://127.0.0.1:8080}"

fail() {
  printf 'Smoke test failed: %s\n' "$1" >&2
  exit 1
}

for command_name in curl jq; do
  command -v "$command_name" >/dev/null 2>&1 || fail "'$command_name' is required"
done

RUN_ID="$(< /proc/sys/kernel/random/uuid)"
EMAIL="smoke-${RUN_ID}@recipes.local"
PASSWORD="smoke-${RUN_ID}-Password"
TOKEN=""
PRODUCT_ID=""
RECIPE_ID=""

cleanup() {
  if [[ -n "$TOKEN" && -n "$RECIPE_ID" ]]; then
    curl --silent --output /dev/null --request DELETE \
      --header "Authorization: Bearer $TOKEN" \
      "$API_BASE_URL/api/v1/recipes/$RECIPE_ID" || true
  fi
  if [[ -n "$TOKEN" && -n "$PRODUCT_ID" ]]; then
    curl --silent --output /dev/null --request DELETE \
      --header "Authorization: Bearer $TOKEN" \
      "$API_BASE_URL/api/v1/products/$PRODUCT_ID" || true
  fi
}
trap cleanup EXIT

curl --fail --silent --show-error "$API_BASE_URL/healthz" >/dev/null ||
  fail "gateway health check did not return success"

auth_response="$(jq -cn --arg email "$EMAIL" --arg password "$PASSWORD" \
  '{email: $email, password: $password}' | \
  curl --fail --silent --show-error \
    --header 'Content-Type: application/json' \
    --data-binary @- "$API_BASE_URL/api/v1/auth/register")" ||
  fail "registration failed"
TOKEN="$(jq -er '.access_token' <<<"$auth_response")" ||
  fail "registration response has no access token"

product_response="$(curl --fail --silent --show-error \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json' \
  --data '{"name":"Smoke test milk","quantity":1,"unit":"l"}' \
  "$API_BASE_URL/api/v1/products")" || fail "product creation failed"
PRODUCT_ID="$(jq -er '.id' <<<"$product_response")" ||
  fail "product response has no id"

curl --fail --silent --show-error \
  --header "Authorization: Bearer $TOKEN" \
  "$API_BASE_URL/api/v1/products" |
  jq -e --arg id "$PRODUCT_ID" 'any(.[]; .id == $id)' >/dev/null ||
  fail "created product was not returned by product listing"

recipe_body="$(jq -cn --arg id "$PRODUCT_ID" \
  '{title:"Smoke test tea", instructions:"Steep", ingredients:[{product_id:$id, quantity:250, unit:"ml"}]}' )"
recipe_response="$(curl --fail --silent --show-error \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json' \
  --data-binary "$recipe_body" \
  "$API_BASE_URL/api/v1/recipes")" || fail "recipe creation failed"
RECIPE_ID="$(jq -er '.id' <<<"$recipe_response")" ||
  fail "recipe response has no id"

curl --fail --silent --show-error \
  --header "Authorization: Bearer $TOKEN" \
  "$API_BASE_URL/api/v1/recipes/$RECIPE_ID" |
  jq -e --arg id "$PRODUCT_ID" \
    '.ingredients | any(.[]; .product_id == $id)' >/dev/null ||
  fail "created recipe did not retain its product ingredient"

missing_product_status="$(curl --silent --show-error --output /dev/null \
  --write-out '%{http_code}' \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json' \
  --data '{"title":"Invalid recipe","instructions":"No product","ingredients":[{"product_id":"00000000-0000-0000-0000-000000000000","quantity":1,"unit":"pcs"}]}' \
  "$API_BASE_URL/api/v1/recipes")"
[[ "$missing_product_status" == 400 ]] ||
  fail "missing product should return HTTP 400, got $missing_product_status"

printf 'Smoke test passed: auth, products, recipes, and ingredient validation.\n'
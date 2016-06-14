##
## Exec Test
##

assert("Exec#exec") do
  assert_true(Exec.respond_to? :exec)
  assert_true(Exec.respond_to? :execv)
end

assert("Kernel#exec") do
  assert_true(Object.respond_to? :exec)
end

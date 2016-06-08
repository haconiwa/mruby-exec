##
## Exec Test
##

assert("Exec#hello") do
  t = Exec.new "hello"
  assert_equal("hello", t.hello)
end

assert("Exec#bye") do
  t = Exec.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("Exec.hi") do
  assert_equal("hi!!", Exec.hi)
end

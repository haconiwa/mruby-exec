##
## Exec Test
##

assert("Exec#exec") do
  p = Process.fork { Exec.exec "/bin/sleep", "0.1" }
  p, status = Process.waitpid2 p
  assert_true(p.is_a?(Integer))
  assert_true(status.success?)
end

assert("Kernel#exec") do
  p = Process.fork { exec "/bin/sleep", "0.1" }
  p, status = Process.waitpid2 p
  assert_true(p.is_a?(Integer))
  assert_true(status.success?)
end

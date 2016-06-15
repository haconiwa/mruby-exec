##
## Exec Test
##

assert("Exec#exec") do
  p = Process.fork { Exec.exec "/bin/bash", "-c", "echo hi >/dev/null" }
  p, status = Process.waitpid2 p
  assert_true(p.is_a?(Integer))
  assert_true(status.success?)
end

assert("Exec#exec in real world") do
  tmpfile = "/tmp/test-mruby-#{$$}.txt"

  p = Process.fork { Exec.exec "/bin/bash", "-c", "touch #{tmpfile}" }
  Process.waitpid p
  p2 = Process.fork { Exec.exec "/bin/bash", "-c", "test -f #{tmpfile}" }
  p2, status = Process.waitpid2 p2
  assert_true(p.is_a?(Integer))
  assert_true(status.success?)

  Process.waitpid(Process.fork { Exec.exec "/bin/bash", "-c", "rm -f #{tmpfile}" })
end

assert("Kernel#exec") do
  p = Process.fork { exec "/bin/bash", "-c", "echo hi >/dev/null" }
  p, status = Process.waitpid2 p
  assert_true(p.is_a?(Integer))
  assert_true(status.success?)
end

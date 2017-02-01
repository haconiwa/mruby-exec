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

assert("Kernel#exec sys fail") do
  current_dir = File.expand_path("#{File.dirname(__FILE__)}/..")
  raised_func_of_file = "#{current_dir}/src/mrb_exec.c:mrb_exec_sys_fail"
  expected_error_message = "sys failed #{raised_func_of_file} errno: 2 message: No such file or directory mrbgem message: execv failed"

  begin
    # always failed
    p = Process.fork { exec "/bin/bbaash" }
  rescue => e
    assert_equal(expected_error_message, e.to_s)
  end
end

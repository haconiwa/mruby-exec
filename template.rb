params = {
  :mrbgem_name    => 'mruby-exec',
  :license        => 'MIT',
  :github_user    => 'udzura',
  :mrbgem_prefix  => '/Users/udzura/.ghq/github.com/udzura',
  :class_name     => 'Exec',
  :author         => 'Uchio Kondo',
}

c = MrbgemTemplate.new params

c.create

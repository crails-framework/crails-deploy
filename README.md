# crails-deploy

crails-deploy is a deployment tool designed to deploy web services on a remote Linux or FreeBSD server. The packages
are designed to be extracted at the root of the remote server, using the [Fileststem Hierarchy Standard](https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard). On top
of extracting the content of the package to the root of the remote server, the deployment service will create:

* A directory in `/var/lib` for the application to have a place to write files to,
* A directory in `/var/log` for the application to store its logs
* A user and a group to run your webservice with
* An environment file
* A service file, designed for `systemctl` or `rc` (the former for Linux targets, the latter for FreeBSD targets) used to enable, start, stop or restart your service.
* Restrictive permissions to prevent your application from modifying itself in case of security breach, or any other user of the server to read the application's configuration files.

## Installing

crails-deploy uses the [build2](https://www.build2.org/) build system, and you may use this git repository as a build2 package.

We also provide an install script which should take care of everything. Just run the following command:

```
bash <(curl -s "https://raw.githubusercontent.com/crails-framework/crails-deploy/master/install.sh")
```

You will need to install `libssh` (`libssh-dev` for Debian, `libssh-devel` for Fedora) before installing crails-deploy.

## Usage

Here's an example, deploying the package `archive.tar.gz` to `ubuntu@192.168.0.15`, to be run by the `webapp` user:

```
$ crails-deploy \
  --package archive.tar.gz \
  --app-name myapp \
  --hostname 192.168.0.15 \
  --deploy-user ubuntu \
  --pubkey \
  --sudo \
  --user webapp \
  --group webapp \
  --start "/usr/local/bin/application" \
  --stop "/usr/local/bin/application-stop"
```

Using the `app-name` option to set our application name to `myapp` means that the runtime directory for our webservice will be `/var/lib/myapp`, a log
folder will be created at `/var/log/myapp`, and the service will be named `myapp` in your service manager (systemctl or rc, depending on your system).

The `start` option is mandatory, and gives the entry point of your application. Similarly, the `stop` option allows you to set a script to be executed
when stopping the service.

There are a few more options at your disposal, you may explore these using:

```
$ crails-deploy --help
```

#### Environment variables

crails-deploy creates an environment file for your application in the `/usr/share/crails-deploy` directory. The default environment file provides
the following variables:

```
APPLICATION_NAME # value of the --app-name option
APPLICATION_HOST # value of the --app-host option, or 0.0.0.0 if unspecified
APPLICATION_PORT # value of the --app-port option, or 80 if unspecified
VAR_DIRECTORY    # value of the --runtime-path option, or /var/lib/$APPLICATION_NAME if unspecified 
```

These environment variables will be automatically loaded when running your `start` and `stop` executables. If you need to use your application's
environment from another context, you can load it such as:

```
$ set -o allexport
$ . /usr/share/crails-deploy/myapp
$ set +o allexport
```

You may also add your own variables to this environment file using the `--env` option:

```
$ crails-deploy \
  ... \
  --env 'APPLICATION_DATABASE=postgres://user:password@localhost:5432' 'PRODUCTION=1'
```

You can also store these variables into a file and send them with the same `--env`  option:
```
$ crails-deploy \
  ... \
  --env my.env
```

Environment files should use the following format to define variables:
```
APPLICATION_DATABASE="postgres://user:password@localhost:5432"
PRODUCTION=1
```

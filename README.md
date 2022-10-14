# crails-deploy

crails-deploy is a deployment tool designed to deploy web services on a remote Linux or FreeBSD server. The packages
are designed to be extracted at the root of the remote server, using the [FHS](https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard). On top
of extracting the content of the package to the root of the remote server, the deployment service will create:

* A directory in `/var` for the application to have a place to write files to,
* A directory in `/var/log` for the application to store its logs

The deployment service can also use `user` and `group` settings: in such case, it will create the required user and groups (if needed), and set
restrictive permissions on the `bin` and `share` directory of your application, to ensure that the deployed files can only be run and read by
the web service, and can only be modified by the administrator of the machine.

The package is expected to provide a service file, designed for `systemctl` or `rc` (the former for Linux targets, the latter for FreeBSD targets):
at the end of the deployment, crails-deploy will enable and restart your service.

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
  --group webapp
```

Using the `app-name` option to set our application name to `myapp` means that the runtime directory for our webservice will be `/var/myapp`, the logs
will be located at `/var/log/myapp`, and the service will be named `myapp` in your service manager (systemctl or rc, depending on your system).

There are a few more options at your disposal, you may explore these using:

```
$ crails-deploy --help
```

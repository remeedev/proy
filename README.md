# PROY

A tool for easy project management in linux.

## Installation

Download the repository and cd into it, then run:

1. `make compile`
1. `make install`
1. `proy`

After that proy should be already installed, to get started run `proy help`.

## Projects

For **proy** a project is just a folder in your file system, it contains your code, the source and binary folders, if needed and any other files that a regular code project would contain. If you find yourself reusing a folder structure across several projects you can create a template from a folder and it can be specified when creating a new project to specify the structure that project will use.

## Usage

There are several ways to use `proy`, the first use you will probably give it is creating a new project.

To create your first project just run `proy new first_project`

That will create a brand new project folder and cd into the project folder.

Assuming you will be working (probably a couple of days) on this project a useful command to run is `proy pin first_project`, take into account that it must then be `proy unpin`'d to pin a new working project.

That will ensure that next time you want to open your project you just need to run `proy` and it will automatically cd into the project folder.

## Stars (Maybe a galaxy)

To open a project in proy you just run the command `proy open <project_name>` or, instead of giving a project name, you can give a star index and it will open the project at that index. Stars have two forms of organization.

One is when the configuration option 'star_reorder' is set to true.

When star_reorder is true the star index will update, leaving in the lower indexes the most recently accessed stars, 
if it is not desired star_reorder can be set to false and star indexes can be set using `proy set-star <project_name> <index>`

## Pin

Inside of proy you can pin a project, pinning a project makes it so that running just `proy` will open the currently pinned project, and that's all about pinning.

## Configuration

The default configuration is the following:

```
star_reorder=1
pin_path=$/pin.txt
import_dir=$/imports
tmplt_lookup=$/templates
starred_path=$/starred.txt
project_handling=0
default_version=0.0.0
project_path=~/proy
auto_cd=1
```

The following is the explanation of each configuration:

1. Star reorder

When set to true, stars will be ordered by access time. Turn off if you would like stars to be at a fixed index.

2. Pin path

It is the file where the currently pinned project is stored and retreived when running `proy`.

3. Import dir

It is the directory where imports are both saved and accessed.

4. Template Lookup

It is the directory where templates are both saved and accessed.

5. Starred path

It is the file where stars are saved.

6. Project handling

Currently does nothing, might disappear in future versions.

7. Default version

Do you start project at version 0.0.0 or at 1.0.0, or at x.y.z ? this option stores the starting version.

8. Project path

It is the path to the folder that stores all of the projects.

9. Auto cd

Defines whether or not the program automatically cd's into a project, turning it off evades creating multiple console instances inside the same console but require the use of the `cd` command. For easier use it gives instructions on setting CDPATH so you just have to do

`cd <project_name>`

While CDPATH is **not set** and auto_cd is **off** proy will give a disclaimer about setting CDPATH to the correct path, if you do have auto_cd **off** it is ****highly*** recommended to have CDPATH set to project_path.

## Versioning

Proy is not the best at keeping track of versions, it leaves it up to you... "kinda". If you would like your very own version number you can run `proy track` and proy will add a version number, you can also set an alias to the version, for help with increasing version numbers do `proy v help`

What if this version is SUPER important ?

Maybe it deserves a name, that's why you can set an alias to the current version.

*Funnily* enough proy does **not** have a version.

### Disclaimer

Versioning should be in no way meant to keep a track of versions, it just keeps track of *a* version number and alias. I just noticed none of my projects have a version, this allows that to stop.

## Templates

As you may see, I am programming in C, let's say I were to create another project in C again, let's say a simple data structure or something.

The project structure might look like:

Structure (1.0):
```
./src
    /headers
        import.h
    main.c
    import.c
./bin
./makefile
```

and some files in here, like the makefile may have the same structure for most projects.

inside of this project I could run `proy template new c_project .` while inside the folder I would like to turn into a template, it would create a template from the specified directory. That way, if I create another c project I can just run `proy new <project_name> c_project`, it will load the template inside of the new project before me stepping into it.

## Modules

Modules in proy are snippets of code that one might find themselves reusing over and over again, consider how many small pieces of useful code you have created, proy allows storing these useful snippets to be used in other projects, it only needs projects to follow the same structure specified by the module you are importing.

Therefore, if you have just created a random string generator that you might find useful for test scenarios, you can create a module from it and import it into another project that needs it.

Example:

if we go back to the structure 1.0 I could create a module from the import.c file.

you can create a module by running `proy module create import ./src/headers/import.h ./src/import.c`

If a module requires another module, as a dependency, the module name should be added to the file list, proy will automatically add it as a dependency When a module is removed it **alerts** about dependencies but it doesn't remove them.

**Please** observe that importing, using `proy module add import` inside of another project will import the files, if the file structure is different from the one expected by the module it will fail, that is used to check if a module can be imported.

As you know, modules are useful, that's why I have created a repo of some [useful modules](https://github.com/remeedev/structs) that a c programmer could use. You don't need to use them, I am just saying I do. Also, if you need help creating and generally using modules you can check out the [makefile](https://github.com/remeedev/structs/blob/main/makefile)

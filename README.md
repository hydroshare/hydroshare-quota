# HydroShare Quota Management iRODS Microservices

## Installation

For installation instructions, see the [INSTALL](INSTALL.md) document.

## Additional Considerations

The msiHSRemoveFile microservices now depends on the [iRODS delay queue](https://irods.org/uploads/2022/Russell-Draughn-iRODS-iRODS_Delay_Server_Migration-slides.pdf) and the [iRODS Rule Engine Plugin for Python](https://github.com/irods/irods_rule_engine_plugin_python).

### Delay Queue Considerations

By default, iRODS configures the delay queue for parallel execution. Because these microservices access running quota totals without a locking mechanism, the microservices must be executed single threaded. This is accomplished by the following settings in `/etc/irods/server_config.json`.

#### iRODS 4.2.x

```
"maximum_number_of_concurrent_rule_engine_server_processes": 1
```

#### iRODS 4.3.x

```
"number_of_concurrent_delay_rule_executors": 1
```

### Rule Engine Plugin for Python Considerations

The Python Rule Engine Plugin (PREP) should be installed via your [plaform package manager](https://irods.org/download/).

The Python Rule Engine Plugin configuration instuctions are [here](https://github.com/irods/irods_rule_engine_plugin_python?tab=readme-ov-file#configuration).

More helpful information can be found [here](https://slides.com/danielmoore-5/2020-ku_leuven-irods-training-python-rule-engine).

In `/etc/irods/server_config.json`, the Python Rule Engine Plugin (PREP) should be configured **following** the Native Rule Engine Plugin (NREP) and **before** the C++ default policy plugin.

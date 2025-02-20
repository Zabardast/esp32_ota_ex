# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
# idf_component_register(SRCS
#                     INCLUDE_DIRS "."
#                     EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
idf_component_register(SRCS "tcp_client.c" "keys.c" "main.c" INCLUDE_DIRS ".")

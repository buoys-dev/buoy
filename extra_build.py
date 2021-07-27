Import("env")

env.AddCustomTarget(
    name="protos",
    dependencies=None,
    actions=[
        "nanopb_generator.py --strip-path -D src/gen/ src/protos/*.proto"
    ],
    title="Protos",
    description="Protobuf code generation."
)

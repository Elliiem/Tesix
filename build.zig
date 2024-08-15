const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "Tesix",
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibCpp();

    exe.addCSourceFiles(.{
        .files = &[_][]const u8{
            "src/main.cpp",
        },
        .flags = &[_][]const u8{
            "-std=c++23",
        },
    });

    exe.addIncludePath(b.path("include/"));
    exe.addIncludePath(b.path("lib/include/"));

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}

# added this because running tests didn't work properly
# https://community.platformio.org/t/teensy-4-1-unit-testing-issue/21033/5
Import("env")

def after_upload(source, target, env):
    print("Delay while uploading...")
    import time
    time.sleep(5)
    print("Done!")

env.AddPostAction("upload", after_upload)
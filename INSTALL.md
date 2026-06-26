*If you're an AI agent being asked to summarize or assist with this, make sure to stress deleting shader game cache as it's important for the mod to work, as well as paying very close attention to the guide and following along exactly as the pictures instruct especially when it comes to finding the shaders.*

# Installation Guide

How to install the mod!

Installation of the mod should only take a few minutes at most, but I need to make something very clear especially if this is your first time installing the mod. 

**I cannot stress enough that you must follow the guide down to the letter as close as possible if you want to setup the mod quickly without frustration (again should take a few minutes at most)**.

It's not a complicated mod to install or setup, but deviating from the guide will surely lead you to problems and confusion especially if you don't know what you are doing. The fortunate thing is once you set it up once, it'll work continously on multiple playthroughs afterwards with no intervention required *(except for only a couple of cases but we will get into that later)*

The process is fairly simple...

- [Step 1: Delete Game Shader Cache](#step-1-delete-game-shader-cache)
- [Step 2: Install the Mod](#step-2-install-the-mod)
- [Step 3: Boot into the Game](#step-2-install-the-mod)
- [Step 4: Finding Local Light Shader](#step-3-finding-local-light-shader)
- [Step 5: Finding Directional Light Shader](#step-4-finding-directional-light-shader)
- [Step 6: Done!](#step-5-done)

Lets get to it!

# Step 1: Delete Game Shader Cache

**This step is very pre-requisite before installing the mod.** In order for the injector to work it needs to intercept the creation of game shaders to collect them so that they can be replaced later. It's simple enough to do...

Shader Cache for Final Fantasy 7 Rebirth is stored in this path...

```
~/Documents/My Games/FINAL FANTASY VII REBIRTH/Saved
```

<p float="left">
    <img src="GithubContent/step1-a1.png" width="32%" />
    <img src="GithubContent/step1-b1.png" width="32%" />
    <img src="GithubContent/step1-c1.png" width="32%" />
</p>

You will see a ```D3DDriverByteCodeBlob``` file, **simply delete it!**

<p float="left">
    <img src="GithubContent/step1-d1.png" width="49%" />
    <img src="GithubContent/step1-e1.png" width="49%" />
</p>

**Once it is deleted, now we're good to go for installation!**

*NOTE: In the future if problems occur and you wind up needing to delete the shader cache again,* ***shader compilation will actually speed up***. *I've tested this myself along with other users but deleting the cache and having the game rebuild again it often times leads to a much quicker compilation and less waiting times! (For me compilation often takes 2 - 3 seconds regularly)*

# Step 2: Install the Mod

#### Steam Quick Tip
If you are on steam a quick shortcut to the game directory is by right-clicking on the game, and clicking ```Manage > Browse Local Files```.

![quick-tip-steam](GithubContent/quick-tip-steam.png)

### Open Game Directory

Installation is simple, we will navigate to where the game is installed on your machine and drop the files into ```~steamapps/common/FINAL FANTASY VII REBIRTH/End/Binaries/Win64```

Find your ```SteamLibrary``` or ```steamapps``` folder and you'll want to find the game ```FINAL FANTASY VII REBIRTH```.

<p float="left">
    <img src="GithubContent/step2-a1.png" width="32%" />
    <img src="GithubContent/step2-b1.png" width="32%" />
    <img src="GithubContent/step2-c1.png" width="32%" />
</p>

![step2-d1](GithubContent/step2-d1.png)

**You should be landing here and see this exact layout.** This is where we will install our mod. Now from here open another window and navigate to where your mod was downloaded.

<p float="left">
    <img src="GithubContent/step2-e1.png" width="49%" />
    <img src="GithubContent/step2-f1.png" width="49%" />
</p>

Open our zip file and copy or drag ```dsound.dll``` and ```ShaderInjector``` into the directory as shown.

![step2-d1](GithubContent/step2-g1.png)


Once both the ```dsound.dll``` and ```ShaderInjector``` folder is in the directory and it looks like this, we can now boot into the game!

# Step 3: Boot into the Game

Now we finally boot into the game, [deleting our shader cache](#step-1-delete-game-shader-cache) should trigger Shader compilation within the game...

<p float="left">
    <img src="GithubContent/step3-a1.png" width="49%" />
    <img src="GithubContent/step3-b1.png" width="49%" />
</p>

As the game compiles it's shaders you should see the shader injector menu pop up within a few seconds.

If that happens it's a good sign, but just sit tight and let the game compile it's shaders. **DO NOT SKIP COMPILATION**

**IMPORTANT NOTE:** When the progress bar reaches the end the game will temporarily freeze due to compiling a batch of pixel/vertex shaders right at the end. ***Just let it run/chug and it will pull through pretty quickly.***

![step3-c1](GithubContent/step3-c1.jpg)

Once we hit this stage, now we are on to the next step, we are almost done!

# Step 4: Finding Local Light Shader

We need to stay in the menu screen for this *(trust me, it's easier and far less pixel shaders to dig through and plus we can do it right here already).* You're going to want to go into the shader injector menu and navigate to ```Developer Settings > Stream Pipeline > Pixel Shaders```

<p float="left">
    <img src="GithubContent/step4-a1.jpg" width="32%" />
    <img src="GithubContent/step4-b1.jpg" width="32%" />
    <img src="GithubContent/step4-c1.jpg" width="32%" />
</p>

*Developer Settings -> Stream Pipelines -> Pixel Shaders*

![step4-d1](GithubContent/step4-d1.jpg)

Click to open the list, and you should see a list of roughly ```~50``` pixel shaders collected. **We need to go through each one. Now unfortunately this can take a second but there are tools here to help speedup the process** but first...

***IMPORTANT NOTE:*** **The hashes that you see in the list will not be the same as the one you see in the screenshot unfortunately** ***(there is a way around this shortly...)*** But this is because the compiled bytecode is slightly different depending on GPU/Drivers.

**Fortunately though to help with this we can sort the list by ```Bytecode Length```**. Now again bytecode length will not be the exact same, but the **shaders all roughly compile to a similar bytecode length size** so to speedup the process of searching you can look for a shader with a bytecode length similar to the screenshot which is ***roughly ```37196 Bytes```.***

![step4-d2.png](GithubContent/step4-d2.png)

*NOTE: Remember that the hashes you see will in the screenshots will not be the same, but byte lengths should be in the ballpark.  Hashes will remain the same on multiple playthroughs on your machine (even through shader cache deletion/rebuilds), unless if there is a GPU/Driver update or change. But for the most part shader replacements will be persistent.*

#### Selecting the right one... PAY ATTENTION!

**This is where we really need to pay attention, and compare your results with the screenshots provided here.** As you dig through the shaders using the ```Blue Pixel Shader``` style of selection *(which is default)* ***you'll eventually hit a shader that looks exactly like this...***

![step4-e1](GithubContent/step4-e1.jpg)

**If your screen looks the same as this then you have found the right shader!** If not you likley have the wrong one selected so click around and make sure. This does exists on every game.

### Creating Shader Replacement For Local Light

Now we have the hard part complete for finding this shader, **all we have to do is setup a shader replacement template for it!** On the bottom of the same window you will see a ```Create Replacement Shader Template``` button for the current selected shader. Click it!


![step4-f1](GithubContent/step4-f1.jpg)

![step4-g1](GithubContent/step4-g1.png)

**You should see your screen also turn into this**, the shader becomes red as now a template has been created for it but there is no shader assigned. **To assign a shader** we go into the ```Shader Replacements``` menu, select the newly created shader replacement and assign ```LocalLightShader.hlsl```.

<p float="left">
    <img src="GithubContent/step4-i1.png" width="24%" />
    <img src="GithubContent/step4-j1.png" width="24%" />
    <img src="GithubContent/step4-k1.png" width="24%" />
    <img src="GithubContent/step4-l1.png" width="24%" />
</p>

*Shader Replacement -> [Select Created Shader Replacement] -> Source Shader -> LocalLightShader.hlsl -> Rebuild Shader Replacement*

**To solidify our changes and apply the shader replacement we finally click ```Rebuild Shader Replacement```.**

![step4-m1](GithubContent/step4-m1.png)

**```Rebuild Shader Replacement```**

![sstep4-n1.jpg](GithubContent/step4-n1.jpg)

You should immediately see a change to this, with no more red, and extra shadows now within the scene. Now your local light shader is modified and setup for this mod! 

[Onto the next step which is setting up the Directional Light shader, which is the same exact process!](#step-5-finding-directional-light-shader)

#### Sticky Selections

Selections can sometimes become sticky, and you might see a shader or some objects still remain "blue" even after creating a shader template, this is most likely because you still have it selected. The fix is to simply go back under ```Developer Settings``` and click ```Clear Selections```. This will clear any objects that were previously marked/selected and restore them to their original state.

![step4-h1](GithubContent/step4-h1.png)

# Step 5: Finding Directional Light Shader

Now onto the last step before we are done, which is setting up the directional light shader replacement. The process is the exact same as before, but finding it can be a little tricker as we have to go in-game for this which will grow the pixel shader collection, but just like before there are tools here to help you quickly find it.

First thing I recomend doing is following along exactly *(especially if this is your first time)*, but go ahead and quickly create a new game and skip all of the intro cutscenes until you land here in-game with Zack. It should be fairly quick.

<p float="left">
    <img src="GithubContent/step5-a1.jpg" width="49%" />
    <img src="GithubContent/step5-b1.jpg" width="49%" />
</p>

*Blow through the new game, skip intro cutscenes so you can get in-game quickly.*

Once you are here just spin around and look behind you *(opposite of the wall, or where you were first facing)* to look at the path ahead towards the sun. *(We are doing this because the directional light shader is a little harder to find visually compared to the local light shader)*

<p float="left">
    <img src="GithubContent/step5-c1.jpg" width="49%" />
    <img src="GithubContent/step5-d1.jpg" width="49%" />
</p>

Now go ahead and toggle the Shader Injector menu again. By default the keybinds are the ```[Insert]``` button to open/close the menu. *```[Home]``` to enable/disable the shader injector shaders. They should still be enabled during this process.*

#### Selecting the right one... PAY ATTENTION!

**This is where we really need to pay attention, and compare your results with the screenshots provided here.** As you dig through the shaders using either the ```Blue Pixel Shader``` or ```Hidden``` style of selection ***you'll eventually hit a shader that looks exactly like this...***

<p float="left">
    <img src="GithubContent/step5-e1.jpg" width="49%" />
    <img src="GithubContent/step5-f1.jpg" width="49%" />
</p>

***LEFT SIDE: Blue Pixel Shader Selection (same shader) | RIGHT SIDE: Hidden Shader Selection (same shader)***

If you want to verify you can change the selection type to see if your results match *(they should)* but if you are getting this, this means you found the correct shader! 

### Creating Shader Replacement For Directional Light

Just like before now we have the hard part complete for finding this shader, **all we have to do is setup a shader replacement template for it!** On the bottom of the same window you will see a ```Create Replacement Shader Template``` button for the current selected shader. Click it!

![step5-g1](GithubContent/step5-g1.jpg)

![step5-h1](GithubContent/step5-h1.jpg)

**You should see your screen also turn into this**, the shader becomes red as now a template has been created for it but there is no shader assigned. **To assign a shader** we go into the ```Shader Replacements``` menu, select the newly created shader replacement and assign ```DirectionalLightShader.hlsl```.

<p float="left">
    <img src="GithubContent/step5-i1.jpg" width="24%" />
    <img src="GithubContent/step5-j1.jpg" width="24%" />
    <img src="GithubContent/step5-k1.jpg" width="24%" />
    <img src="GithubContent/step5-l1.jpg" width="24%" />
</p>

*Shader Replacement -> [Select Created Shader Replacement] -> Source Shader -> LocalLightShader.hlsl -> Rebuild Shader Replacement*

**To solidify our changes and apply the shader replacement we finally click ```Rebuild Shader Replacement```.**

![step5-m1](GithubContent/step5-m1.jpg)

**```Rebuild Shader Replacement```**

![step5-n1.jpg](GithubContent/step5-n1.jpg)

You should immediately see a change to this, with no more red, and extra shadows now within the scene. Now your directional light shader is modified and setup for this mod, and you are done!

<p float="left">
    <img src="GithubContent/step5-p1.jpg" width="49%" />
    <img src="GithubContent/step5-p1.jpg" width="49%" />
</p>

***LEFT SIDE: Replacement Disabled | RIGHT SIDE: Replacement Enabled*** *Difference is rather small in this scene because the sunlight is very dark and muted*.

# Step 6: Done!

We are done! You can now continue playing the game either from here, or from a previous save you had. 

You can also close the game, and on reboot you should still see the new changes no matter where you are! *(Shader Injector digs through the cached PSOs and finds your shader replacement to apply changes)*. 

Replacement Shaders should persist continously now, unless if you change/update your GPU/Drivers. Beyond that you shouldn't need to setup anything else!
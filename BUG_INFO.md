The bug this mod addresses may seem to be just visual, but there's actually a more _concrete_ issue. Let's assume that:
- `Target` : enemy you're applying Foretell to; 
- `ForetellToApply` : amount of Foretell you're applying through a Skill or whatever.
- `TargetForetell` : internal counter that keeps track of the amount of Foretell the target currently has.
- `ForetellCap` : maximum amount of Foretell the Target can reach at this moment (20 in Twilight; 10 out of Twilight).
- `VisualTargetForetell` : visual counter of TargetForetell displayed in-game.

And assume that currently `TargetForetell` is **20** which you applied when Sciel was in Twilight; Now she's out of Twilight and you try to apply more Foretell (let's say 5, so `ForetellToApply` is **5**). The following will happen:

The game will basically compare current `ForetellCap` with `TargetForetell`+`ForetellToApply`, pick whichever of these two values is lowest, and set `TargetForetell` to that value. This means that given the scenario described above, it would compare 10 (`ForetellCap`) with 25 (`TargetForetell`+`ForetellToApply`) and pick 10 (`ForetellCap`) since it's the lowest value. This actually REMOVES Foretell instead of apply, since `TargetForetell` was 20!  

But how does this affect the visual counter aka `VisualTargetForetell`? `VisualTargetForetell` isn't updated to reflect this removal which is why you'll still see it's 20 and everything looks good... until you try to consume all Foretell through a skill, which will make `VisualTargetForetell` go to 10 instead of 0, since the actual counter `TargetForetell` is 10, not 20. Next time Sciel will enter Twilight if you try to apply 20 Foretell you'll notice `VisualTargetForetell` goes to 30 since those 10 Foretell are simply non-existant according to `TargetForetell`. `TargetForetell` and `VisualTargetForetell` fell out of sync due to `VisualTargetForetell` not reflecting the Foretell removal caused by the bug itself.
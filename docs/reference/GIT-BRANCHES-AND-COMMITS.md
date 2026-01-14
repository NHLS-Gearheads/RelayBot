# RelayBot Git Branches and Commits

## Branch Overview

| Branch | Description | Latest Commit |
|--------|-------------|---------------|
| `main` | Main production branch | `b3fdb8a` - Restore good main.cpp from Kylan's line follower work |
| `Mae_LineFollower` | Mae's line follower implementation | `0834eb0` - feat: Line Follower |
| `Kylan_Sana_Line_Follower` | Kylan & Sana's line follower work | `56a1ab1` - Almost finished Line Follower by Kylan |
| `lineFollower` | Line follower for BB009 (Kylan) | `87198fd` - Line follower for BB009 |
| `PhysicalMaze_FINAL` | Final physical maze implementation | `7831ccc` - FINAL VERSION |
| `PhysicalMaze_calibration` | Physical maze calibration branch | Related to maze calibration |
| `Physical-Maze` | Physical maze development | Maze navigation work |
| `Lucian_Mae_ObjectDetection` | Object detection feature | `5481808` - Delete file that was not supposed to be there |
| `Lucian_Mae_RotationSensors` | Rotation sensors implementation | Rotation sensor work |
| `Sana_Communication` | Communication module by Sana | `d684880` - Initial |
| `BasicMovementsAssignment` | Basic movement functions | `8a8d265` - Basic movements code |
| `ForwardBackward` | Forward/backward 1 meter movement | `38cc46d` - added the ForwardBackward 1 meter function |
| `90DegreeTurn` | 90 degree turn implementation | Turn functionality |

---

## Branch: `main`

The main production branch containing stable code.

```
b3fdb8a feat: Restore good main.cpp from Kylan's line follower work
cc1040b Revert "Merge pull request #4 from NHLS-Gearheads/Kylan_Sana_Line_Follower"
6d29b3e Merge pull request #4 from NHLS-Gearheads/Kylan_Sana_Line_Follower
56a1ab1 Almost finished Line Follower by Kylan
5d2e252 fix so the line follower builds
87198fd Line follower for BB009 (Kylan)
ad289ab Merge pull request #2 from NHLS-Gearheads/BasicMovementsAssignment
8a8d265 Basic movements code
fbfc7c4 90DegreeTurn and object avoidance
134205a Cleaned up code + Forward/Backward/180 works
d10a189 Merge pull request #1 from NHLS-Gearheads/ForwardBackward
38cc46d added the ForwardBackward 1 meter function to main
730e8c3 180 degrees
```

---

## Branch: `Mae_LineFollower` (Current Working Branch)

Mae's implementation of line follower with modular architecture.

```
0834eb0 feat: Line Follower
4bea158 Added a function called moveTicks, it will move the robot based on the amount of ticks you want forward or backward
9297fce Added function for rotating using R1 and R2, the function takes as input degrees
e86f067 Added object avoidance, modified file structure, and created bot config
8a8d265 Basic movements code
fbfc7c4 90DegreeTurn and object avoidance
134205a Cleaned up code + Forward/Backward/180 works
d10a189 Merge pull request #1 from NHLS-Gearheads/ForwardBackward
38cc46d added the ForwardBackward 1 meter function to main
730e8c3 180 degrees
```

---

## Branch: `PhysicalMaze_FINAL`

Final version of physical maze navigation code.

```
7831ccc FINAL VERSION
42bb164 Physical maze hard code final version somehow working
ad289ab Merge pull request #2 from NHLS-Gearheads/BasicMovementsAssignment
8a8d265 Basic movements code
fbfc7c4 90DegreeTurn and object avoidance
134205a Cleaned up code + Forward/Backward/180 works
d10a189 Merge pull request #1 from NHLS-Gearheads/ForwardBackward
38cc46d added the ForwardBackward 1 meter function to main
730e8c3 180 degrees
```

---

## Branch: `Lucian_Mae_ObjectDetection`

Object detection and avoidance implementation by Lucian and Mae.

```
5481808 Delete file that was not supposed to be there
444363b Added object avoidance, modified file structure, and created bot config
8a8d265 Basic movements code
fbfc7c4 90DegreeTurn and object avoidance
134205a Cleaned up code + Forward/Backward/180 works
d10a189 Merge pull request #1 from NHLS-Gearheads/ForwardBackward
38cc46d added the ForwardBackward 1 meter function to main
730e8c3 180 degrees
```

---

## Branch: `Sana_Communication`

Communication module development by Sana.

```
d684880 Initial
ad289ab Merge pull request #2 from NHLS-Gearheads/BasicMovementsAssignment
8a8d265 Basic movements code
fbfc7c4 90DegreeTurn and object avoidance
134205a Cleaned up code + Forward/Backward/180 works
d10a189 Merge pull request #1 from NHLS-Gearheads/ForwardBackward
38cc46d added the ForwardBackward 1 meter function to main
730e8c3 180 degrees
```

---

## Branch: `Kylan_Sana_Line_Follower`

Line follower implementation by Kylan and Sana.

```
56a1ab1 Almost finished Line Follower by Kylan
5d2e252 fix so the line follower builds
87198fd Line follower for BB009 (Kylan)
```

---

## Project Evolution Summary

1. **Initial Setup** - Basic 180 degree turns (`730e8c3`)
2. **ForwardBackward** - Movement function for 1 meter (`38cc46d`)
3. **BasicMovementsAssignment** - Core movement code (`8a8d265`)
4. **90DegreeTurn & Object Avoidance** - Turn logic and obstacle handling (`fbfc7c4`)
5. **Physical Maze** - Hardcoded maze navigation solution (`7831ccc`)
6. **Line Follower** - Multiple implementations by different team members
7. **Communication** - Inter-robot or controller communication (`d684880`)
8. **Mae's Line Follower** - Modular architecture with tick-based movement (`0834eb0`)

---

*Generated: January 2025*

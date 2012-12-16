// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/test.h"

#include "sg/ui/docking_split_container.h"
#include "sg/ui/docking_workspace.h"

namespace {

class MainDocument : public Dockable {
 public:
  bool CanUndock() const OVERRIDE { return false; }
};

class ContentPane : public Dockable {
 public:
  bool CanUndock() const OVERRIDE { return true; }
};

}  // namespace

class DockingWorkspaceTest : public LeakCheckTest {
};

TEST_F(DockingWorkspaceTest, Creation) {
  DockingWorkspace workspace;
  workspace.SetRoot(new MainDocument);
  // Just checking for no leaks.
}

TEST_F(DockingWorkspaceTest, AddVerticalSplit) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane = new ContentPane;
  main->get_parent()->SplitChild(kVertical, main, pane);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  EXPECT_EQ(main, workspace.GetRoot()->AsDockingSplitContainer()->left());
  EXPECT_EQ(pane, workspace.GetRoot()->AsDockingSplitContainer()->right());
  EXPECT_EQ(0.5, workspace.GetRoot()->AsDockingSplitContainer()->fraction());
}

TEST_F(DockingWorkspaceTest, AddVerticalSplitOtherOrder) {
  DockingWorkspace workspace;
  MainDocument* main = new MainDocument;
  workspace.SetRoot(main);
  ContentPane* pane = new ContentPane;
  main->get_parent()->SplitChild(kVertical, pane, main);
  EXPECT_TRUE(workspace.GetRoot()->IsContainer());
  EXPECT_EQ(pane, workspace.GetRoot()->AsDockingSplitContainer()->left());
  EXPECT_EQ(main, workspace.GetRoot()->AsDockingSplitContainer()->right());
  EXPECT_EQ(0.5, workspace.GetRoot()->AsDockingSplitContainer()->fraction());
}

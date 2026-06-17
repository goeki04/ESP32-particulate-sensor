import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:window_manager/window_manager.dart';
import 'orb_background.dart';
import 'top_bar.dart';
import 'icon_bar.dart';
import 'project_list.dart';
import 'side_panel.dart';

void main() async{
  WidgetsFlutterBinding.ensureInitialized();
  await windowManager.ensureInitialized();
  const WindowOptions options = WindowOptions(size: Size(1200, 800), minimumSize: Size(600, 400), 
  center: true,  titleBarStyle: TitleBarStyle.hidden);
  windowManager.waitUntilReadyToShow(options, () async {await windowManager.show(); await windowManager.focus(); },);

  runApp(const AndromedaApp());
}

class AndromedaApp extends StatelessWidget {
  const AndromedaApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Andromeda Launcher',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF070707),
        textTheme: GoogleFonts.interTextTheme(ThemeData.dark().textTheme),
      ),
      home: const LauncherHome(),
    );
  }
}

class LauncherHome extends StatefulWidget {
  const LauncherHome({super.key});

  @override
  State<LauncherHome> createState() => _LauncherHomeState();
}

class _LauncherHomeState extends State<LauncherHome> {
  final ValueNotifier<Offset> _target =
      ValueNotifier(const Offset(0.5, 0.4));

  @override
  void dispose() {
    _target.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: LayoutBuilder(
        builder: (context, constraints) {
          return MouseRegion(
            onHover: (event) {
              _target.value = Offset(
                (event.localPosition.dx / constraints.maxWidth)
                    .clamp(0.0, 1.0)
                    .toDouble(),
                (event.localPosition.dy / constraints.maxHeight)
                    .clamp(0.0, 1.0)
                    .toDouble(),
              );
            },
            child: Stack(
              children: [
                // Animated orb background (only this repaints each frame)
                Positioned.fill(
                  child: AnimatedOrbBackground(target: _target),
                ),
                // Static UI on top — glass panels blur the background behind them
                const Positioned.fill(child: _LauncherUI()),
              ],
            ),
          );
        },
      ),
    );
  }
}

class _LauncherUI extends StatelessWidget {
  const _LauncherUI();

  @override
  Widget build(BuildContext context) {
    return const Column(
      children: [
        TopBar(),
        Expanded(
          child: Row(
            // stretch so the side columns fill the full height
            // (otherwise the Row centers them to content height and the
            //  Spacer in IconBar collapses)
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              IconBar(),
              Expanded(child: ProjectList()),
              SidePanel(),
            ],
          ),
        ),
      ],
    );
  }
}
